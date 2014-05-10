/*
 * CCouncilMember.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */

#include "CCouncilMember.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"
#include "engine/core/mode/CGameMode.h"
#include <base/utils/misc.h>
#include "sdl/audio/Audio.h"
#include "sdl/audio/music/CMusic.h"
#include "fileio/KeenFiles.h"
#include <typeinfo>

namespace galaxy {

const int ELDER_MOVE_SPEED = 1;
const int ELDER_MOVE_TIMER = 10;

int NumberToRescue;


#define A_COUNCIL_MEMBER_MOVE		0	/* Ordinary walking council member. */
#define A_COUNCIL_MEMBER_THINK		2	/* Council member stopping to ponder. */



CCouncilMember::CCouncilMember(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0),
rescued(false),
m_timer(0)
{
	setupGalaxyObjectOnMap(0x1FB8, A_COUNCIL_MEMBER_MOVE);
	mp_processState = &CCouncilMember::processWalking;
	
	answermap[0] = "KEEN_NOSWEAT_TEXT";
	answermap[1] = "KEEN_BEARDED_ONE_TEXT";
	answermap[2] = "KEEN_NO_PROBLEMO_TEXT";
	answermap[3] = "KEEN_GREAT_TEXT";
	answermap[4] = "KEEN_LOOKS_LIKE_SAME_GUY_TEXT";
	answermap[5] = "KEEN_GOOD_IDEA_GRAMPS";
	answermap[6] = "KEEN_ROAD_RISE_FEET_TEXT";
	answermap[7] = "KEEN_WISE_PLAN_TEXT";
	answermap[8] = "KEEN_LAST_ELDER_TEXT";
	
	NumberToRescue = 0;
    byte *ptr = gKeenFiles.exeFile.getRawData();
	ptr += 0x6AE6;
	memcpy(&NumberToRescue, ptr, 1 );
}




void CCouncilMember::processWalking()
{
	performCollisions();
	performGravityLow();

	// Check if there is a cliff and move him back in case
	performCliffStop(m_Action.velX<<1);


	if( m_timer < ELDER_MOVE_TIMER )
	{
		m_timer++;
		return;
	}
	else
	{
		m_timer = 0;
	}

	// Chance if he will think
	if( getProbability(25) )
	{
		m_timer = 0;
		mp_processState = &CCouncilMember::processThinking;
		setAction(A_COUNCIL_MEMBER_THINK);
		return;
	}

	// Move normally in the direction
	if( xDirection == RIGHT )
		moveRight( m_Action.velX<<1 );
	else
		moveLeft( m_Action.velX<<1 );

}



void CCouncilMember::processThinking()
{
	if( getActionStatus(A_COUNCIL_MEMBER_MOVE) )
	{
		setAction(A_COUNCIL_MEMBER_MOVE);
		mp_processState = &CCouncilMember::processWalking;
	}
}




void CCouncilMember::process()
{
	(this->*mp_processState)();

	if( blockedl )
		xDirection = RIGHT;
	else if(blockedr)
		xDirection = LEFT;

	if(!processActionRoutine())
			exists = false;
}


void CCouncilMember::performJanitorMode()
{
	std::string elder_text[4];

	elder_text[0] = g_pBehaviorEngine->getString("JANITOR_TEXT1");
	elder_text[1] = g_pBehaviorEngine->getString("JANITOR_TEXT2");
	elder_text[2] = g_pBehaviorEngine->getString("JANITOR_TEXT3");
	elder_text[3] = g_pBehaviorEngine->getString("JANITOR_TEXT4");

    std::vector<CMessageBoxGalaxy*> msgs;

    msgs.push_back( new CMessageBoxBitmapGalaxy(elder_text[0], gGraphics.getBitmapFromId(104), LEFT) );
    msgs.push_back( new CMessageBoxBitmapGalaxy(elder_text[1], gGraphics.getBitmapFromId(104), LEFT) );
    msgs.push_back( new CMessageBoxBitmapGalaxy(elder_text[2], *gGraphics.getBitmapFromStr("KEENTALKING"), RIGHT) );
    msgs.push_back( new CMessageBoxBitmapGalaxy(elder_text[3], gGraphics.getBitmapFromId(104), LEFT) );

    showMsgVec( msgs );

	rescued = true;
}


void CCouncilMember::getTouchedBy(CSpriteObject &theObject)
{
	if(rescued)
		return;

	// When Keen touches the Council Member exit the level and add one to the council list
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		int &rescuedelders = player->m_Inventory.Item.m_special.ep4.elders;

		// TODO: In this part we have to check which level we are and send the proper messages

		if(mp_Map->isSecret)
		{
			performJanitorMode();
			return;
		}


		g_pSound->playSound(SOUND_RESCUE_COUNCIL_MEMBER, PLAY_PAUSEALL);
        gEventManager.add( new EventPlayTrack(5) );

		std::string elder_text[2];

		if( mp_Map->getLevel() == 17 ) // Under water the text is a bit different
		{
			elder_text[0] = g_pBehaviorEngine->getString("ELDERS_UNDERWATER_TEXT");
			elder_text[1] = "";
		}
		else
		{
			elder_text[0] = g_pBehaviorEngine->getString("ELDERS_TEXT");
			elder_text[1] = g_pBehaviorEngine->getString(answermap[rescuedelders]);
		}


        std::vector<CMessageBoxGalaxy*> msgs;

        msgs.push_back( new CMessageBoxBitmapGalaxy(elder_text[0], gGraphics.getBitmapFromId(104), LEFT) );

		rescuedelders++;
		
		if(rescuedelders >= NumberToRescue) // In this case the game ends.
		{
            msgs.push_back( new CMessageBoxBitmapGalaxy(elder_text[1],
                                    *gGraphics.getBitmapFromStr("KEENTHUMBSUP"),
                                    RIGHT) );


            msgs.push_back( new CMessageBoxBitmapGalaxy(g_pBehaviorEngine->getString(answermap[8]),
                            *gGraphics.getBitmapFromStr("KEENTHUMBSUP"), RIGHT) );



		    
		    const std::string end_text("End of Episode.\n"
					       "The game will be restarted.\n"
					       "You can replay it again or\n" 
					       "try another Episode for more fun!\n"
					       "The original epilog is under construction.");

            msgs.push_back( new CMessageBoxGalaxy(end_text, new EventEndGamePlay()) );

		}
		else
		{
            msgs.push_back( new CMessageBoxBitmapGalaxy(
                                elder_text[1],
                                *gGraphics.getBitmapFromStr("KEENTHUMBSUP"),
                                RIGHT,
                                new EventExitLevel(mp_Map->getLevel(), true, false, mSprVar)) );
		}

        showMsgVec( msgs );

        player->m_Inventory.Item.m_gem.clear();

		rescued = true;
	}
}

}
