/*
 * CPoisonSlug.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */


#include "CPoisonSlug.h"
#include "CSlugSlime.h"
#include "engine/galaxy/ai/CPlayerBase.h"
#include "misc.h"

namespace galaxy {


const int SLUG_MOVE_SPEED = 1;
const int SLUG_MOVE_TIMER = 10;

CPoisonSlug::CPoisonSlug(CMap *pmap, Uint32 x, Uint32 y,
						std::vector<CObject*>&ObjectPtrs) :
CObject(pmap, x, y, OBJ_NONE),
CStunnable(pmap, x, y, OBJ_NONE),
m_ObjectPtrs(ObjectPtrs),
m_timer(0)
{
	setupGalaxyObjectOnMap(0x2012, A_SLUG_MOVE);

	mp_processState = (void (CStunnable::*)()) &CPoisonSlug::processCrawling;
	m_hDir = LEFT;
}





void CPoisonSlug::processCrawling()
{
	performCollisions();
	processFalling();


	// Check if there is a cliff
	performCliffStop(m_Action.H_anim_move_amount<<1);


	if( m_timer < SLUG_MOVE_TIMER )
	{
		m_timer++;
		return;
	}
	else
	{
		m_timer = 0;
	}

	// Chance to poo
	if( getProbability(30) )
	{
		m_timer = 0;
		mp_processState = (void (CStunnable::*)()) &CPoisonSlug::processPooing;
		setAction( A_SLUG_POOING );
		playSound( SOUND_SLUG_DEFECATE );
		m_ObjectPtrs.push_back(new CSlugSlime(mp_Map, getXLeftPos(), getYDownPos()-(8<<STC)));
		return;
	}

	// Move normally in the direction
	if( m_hDir == RIGHT )
		moveRight( m_Action.H_anim_move_amount<<1 );
	else
		moveLeft( m_Action.H_anim_move_amount<<1 );

}





void CPoisonSlug::processPooing()
{
	if( getActionStatus(A_SLUG_MOVE) )
	{
		setAction(A_SLUG_MOVE);
		mp_processState = (void (CStunnable::*)()) &CPoisonSlug::processCrawling;
	}
}


void CPoisonSlug::getTouchedBy(CObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		mp_processState = &CStunnable::processGettingStunned;
		setAction( rand()%2 ? A_SLUG_STUNNED : A_SLUG_STUNNED_ALT );
		dead = true;
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}



void CPoisonSlug::process()
{
	(this->*mp_processState)();

	if( blockedl )
		m_hDir = RIGHT;
	else if(blockedr)
		m_hDir = LEFT;

	processActionRoutine();
}

}
