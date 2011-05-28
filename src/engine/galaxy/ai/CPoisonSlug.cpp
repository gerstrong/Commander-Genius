/*
 * CPoisonSlug.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */

#include "CPoisonSlug.h"
#include "misc.h"

namespace galaxy {


const int SLUG_MOVE_SPEED = 1;
const int SLUG_MOVE_TIMER = 10;
//const int SLUG_POO_TIME = 1000;

CPoisonSlug::CPoisonSlug(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE),
m_timer(0)
{
	m_ActionBaseOffset = 0x2012;
	m_hDir = LEFT;
	setActionForce(A_SLUG_MOVE);
	mp_processState = &CPoisonSlug::processCrawling;
	processActionRoutine();
	performCollisions();
}

void CPoisonSlug::processCrawling()
{
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
	if( getProbability(50) )
	{
		m_timer = 0;
		mp_processState = &CPoisonSlug::processPooing;
		setAction(A_SLUG_POOING);
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
		// TODO: Create that slime
		setAction(A_SLUG_MOVE);
		mp_processState = &CPoisonSlug::processCrawling;
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
