/*
 * CCouncilMember.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */

#include "CCouncilMember.h"
#include "CPlayerLevel.h"
#include "misc.h"
#include "sdl/sound/CSound.h"
#include "sdl/music/CMusic.h"

namespace galaxy {

const int ELDER_MOVE_SPEED = 1;
const int ELDER_MOVE_TIMER = 10;


CCouncilMember::CCouncilMember(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE),
rescued(false),
m_timer(0)
{
	m_ActionBaseOffset = 0x1FB8;
	setActionForce(A_COUNCIL_MEMBER_MOVE);
	mp_processState = &CCouncilMember::processWalking;
	setActionSprite();
	calcBouncingBoxes();
}




void CCouncilMember::processWalking()
{
	performCollisions();
	processFalling();


	// Check if there is a cliff and move him back in case
	performCliffStop(m_Action.H_anim_move_amount<<1);


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
	if( m_hDir == RIGHT )
		moveRight( m_Action.H_anim_move_amount<<1 );
	else
		moveLeft( m_Action.H_anim_move_amount<<1 );

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
		m_hDir = RIGHT;
	else if(blockedr)
		m_hDir = LEFT;

	processActionRoutine();
}




void CCouncilMember::getTouchedBy(CObject &theObject)
{
	if(rescued)
		return;

	if(hitdetect(theObject))
	{
		// When Keen touches the Council Member exit the level and add one to the council list
		if(theObject.m_type == OBJ_PLAYER)
		{
			CPlayerLevel &Player = static_cast<CPlayerLevel&>(theObject);

			CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;


			// TODO: Also we need to play the elder sound and the proper Music to it!
			// TODO: In this part we have to check which level we are and send the proper messages


			g_pSound->playSound(SOUND_RESCUE_COUNCIL_MEMBER, PLAY_PAUSEALL);
			//g_pMusicPlayer->LoadfromMusicTable();

			std::string elder_text[2];
			elder_text[0] = g_pBehaviorEngine->getString("ELDERS_TEXT");
			elder_text[1] = g_pBehaviorEngine->getString("KEEN_NOSWEAT_TEXT");
			EventContainer.add( new EventSendBitmapDialogMsg(104, elder_text[0], LEFT) );
			EventContainer.add( new EventSendBitmapDialogMsg(106, elder_text[1], RIGHT) );


			EventContainer.add( new EventExitLevel(mp_Map->getLevel(), true) );
			Player.m_Inventory.Item.m_special.ep4.elders++;

			rescued = true;
		}
	}

}

}
