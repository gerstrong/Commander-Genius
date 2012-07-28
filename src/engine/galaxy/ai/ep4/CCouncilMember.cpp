/*
 * CCouncilMember.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */

#include "CCouncilMember.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "misc.h"
#include "sdl/sound/CSound.h"
#include "sdl/music/CMusic.h"
#include <typeinfo>

namespace galaxy {

const int ELDER_MOVE_SPEED = 1;
const int ELDER_MOVE_TIMER = 10;


CCouncilMember::CCouncilMember(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
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




void CCouncilMember::getTouchedBy(CSpriteObject &theObject)
{
	if(rescued)
		return;

	// When Keen touches the Council Member exit the level and add one to the council list
	//if( typeid(theObject) == typeid(CPlayerLevel) )
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		int &rescuedelders = player->m_Inventory.Item.m_special.ep4.elders;

		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

		// TODO: In this part we have to check which level we are and send the proper messages

		g_pSound->playSound(SOUND_RESCUE_COUNCIL_MEMBER, PLAY_PAUSEALL);
		EventContainer.add( new EventPlayTrack(5) );

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


		std::vector< SmartPointer<EventSendBitmapDialogMsg> > msgs;

		msgs.push_back( new EventSendBitmapDialogMsg(104, elder_text[0], LEFT) );
		msgs.push_back( new EventSendBitmapDialogMsg(106, elder_text[1], RIGHT) );


		if(rescuedelders == 7)
			msgs.push_back( new EventSendBitmapDialogMsg(106, g_pBehaviorEngine->getString(answermap[8]), RIGHT) );


		EventContainer.add( new EventSendBitmapDialogMessages(msgs) );

		EventContainer.add( new EventExitLevel(mp_Map->getLevel(), true) );
		rescuedelders++;

		rescued = true;
	}
}

}
