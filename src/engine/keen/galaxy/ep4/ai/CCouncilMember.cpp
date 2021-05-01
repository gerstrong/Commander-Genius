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
#include <base/audio/Audio.h>
#include <base/audio/music/CMusic.h>
#include "fileio/KeenFiles.h"
#include "../../menu/ComputerWrist.h"
#include <typeinfo>

namespace galaxy {

//const int ELDER_MOVE_SPEED = 1;
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
    gs_byte *ptr = gKeenFiles.exeFile.getRawData();
	ptr += 0x6AE6;
	memcpy(&NumberToRescue, ptr, 1 );

    const auto fullFName = JoinPaths(gKeenFiles.gameDir, "council.lua");
    mLua.loadFile(fullFName);
}




void CCouncilMember::processWalking()
{
	performCollisions();
	performGravityLow();

	// Check if there is a cliff and move him back in case
    performCliffStop(m_Action.h_anim_move<<1);


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
		moveRight( m_Action.h_anim_move<<1 );
	else
		moveLeft( m_Action.h_anim_move<<1 );

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

	elder_text[0] = gBehaviorEngine.getString("JANITOR_TEXT1");
	elder_text[1] = gBehaviorEngine.getString("JANITOR_TEXT2");
	elder_text[2] = gBehaviorEngine.getString("JANITOR_TEXT3");
	elder_text[3] = gBehaviorEngine.getString("JANITOR_TEXT4");

    std::vector<CMessageBoxGalaxy*> msgs;

    int sprVar = 0;

    msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, elder_text[0],
                    gGraphics.getBitmapFromId(sprVar, 104), LEFT, false, nullptr) );
    msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, elder_text[1],
                    *gGraphics.getBitmapFromStr(sprVar, "KEENTALKING"), RIGHT, false, nullptr) );
    msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, elder_text[2],
                    gGraphics.getBitmapFromId(sprVar, 104), LEFT, false, nullptr) );
    msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, elder_text[3],
                    *gGraphics.getBitmapFromStr(sprVar, "KEENTALKING"), RIGHT, false, nullptr) );

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
		int &rescuedelders = player->m_Inventory.Item.m_special.elders;

		// TODO: In this part we have to check which level we are and send the proper messages

		if(mpMap->isSecret)
        {
            player->m_Inventory.addAchievementTask("Clogged Toilet", 1);
			performJanitorMode();
			return;
		}


        playSound(SOUND_RESCUE_COUNCIL_MEMBER, SoundPlayMode::PLAY_PAUSEALL);
        gEventManager.add( new EventPlayTrack(5) );        

		std::string elder_text[2];

        // Lua custom dialogs
        bool customDlgs = false;

        if(mLua)
        {
            std::string value;
            customDlgs = true;

            int level = mpMap->getLevel();

            auto ok = mLua.runFunctionRetOneStr("getMemberDialog", level, value);

            if(ok) elder_text[0] = value;

            ok = mLua.runFunctionRetOneStr("getMemberAnswer", level, value);

            if(ok) elder_text[1] = value;
        }

        if(!customDlgs)
        {
            if( mpMap->getLevel() == 17 ) // Under water the text is a bit different
            {
                elder_text[0] = gBehaviorEngine.getString("ELDERS_UNDERWATER_TEXT");
                elder_text[1] = "";
            }
            else
            {
                elder_text[0] = gBehaviorEngine.getString("ELDERS_TEXT");
                elder_text[1] = gBehaviorEngine.getString(answermap[rescuedelders]);
            }
        }

        std::vector<CMessageBoxGalaxy*> msgs;

        const int sprVar = player->getSpriteVariantIdx();

        msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, elder_text[0],
                        gGraphics.getBitmapFromId(sprVar, 104), LEFT, false, nullptr) );

		rescuedelders++;
		
		if(rescuedelders >= NumberToRescue) // In this case the game ends.
		{
            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, elder_text[1],
                                    *gGraphics.getBitmapFromStr(sprVar, "KEENTHUMBSUP"),
                                    RIGHT, false, nullptr) );


            const auto lastStr = gBehaviorEngine.getString(answermap[8]);

            auto joinEvents = []()
            {
                gEventManager.add(new EventEndGamePlay());
                gEventManager.add(new OpenComputerWrist(4, false, false, 3, true));
            };

            msgs.push_back( new CMessageBoxBitmapGalaxy(sprVar, lastStr,
                            *gGraphics.getBitmapFromStr(sprVar, "KEENTHUMBSUP"),
                            RIGHT, false, new FunctionToEvent(joinEvents)) );
		}
		else
		{
            auto evExit = new EventExitLevel(mpMap->getLevel(),
                                             true, false,
                                             player->getPlayerIdx());
            evExit->playSound = true;

            msgs.push_back( new CMessageBoxBitmapGalaxy(
                                sprVar, elder_text[1],
                                *gGraphics.getBitmapFromStr(sprVar, "KEENTHUMBSUP"),
                                RIGHT,
                                false,
                                evExit) );
		}

        showMsgVec( msgs );

        player->m_Inventory.Item.m_gem.clear();

		rescued = true;
	}
}

}
