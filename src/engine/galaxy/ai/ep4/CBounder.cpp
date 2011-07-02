/*
 * CBounder.cpp
 *
 *  Created on: 02.07.2011
 *      Author: gerstrong
 */

#include "CBounder.h"

namespace galaxy
{

#define A_BOUNDER_BOUNCE	0
#define A_BOUNDER_MOVE		2
//#define A_BOUNDER_ONFLOOR	4
#define A_BOUNDER_STUNNED	5

const int MAX_BOUNCE_BOOST = -80;
const int HOR_SPEED = 40;

CBounder::CBounder(CMap *pmap, Uint32 x, Uint32 y) :
CStunnable(pmap, x, y),
bounceboost(0)
{
	m_ActionBaseOffset = 0x2F12;
	setActionForce(A_BOUNDER_BOUNCE);
	mp_processState = (void (CStunnable::*)()) &CBounder::processBounce;
	m_hDir = NONE;
	performCollisions();
}

void CBounder::getTouchedBy(CObject &theObject)
{
	if(dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		mp_processState = &CStunnable::processGettingStunned;
		setAction( A_BOUNDER_STUNNED );
		dead = true;
	}
}

void CBounder::processBounce()
{
	// When bounder hits the floor start the inertia again.
	if(blockedd || onslope)
	{
		yinertia = MAX_BOUNCE_BOOST;
		playSound( SOUND_KEEN_POGO );

		// Decide whether go left, right or just bounce up.
		// This is will depend on the position of the player
		switch( rand() % 3 )
		{
		case 0:
			setAction( A_BOUNDER_MOVE+1 );
			m_hDir = LEFT;
			break;
		case 2:
			setAction( A_BOUNDER_MOVE );
			m_hDir = RIGHT;
			break;
		default:
			setAction( A_BOUNDER_BOUNCE );
			m_hDir = NONE;
			break;
		}
	}

	moveYDir(yinertia);


	if(m_hDir == LEFT)
		moveLeft(HOR_SPEED);
	else if(m_hDir == RIGHT)
		moveRight(HOR_SPEED);
}

void CBounder::process()
{
	// Bounce
	performCollisions();
	processFalling();


	(this->*mp_processState)();

	if( blockedl )
		m_hDir = RIGHT;
	else if(blockedr)
		m_hDir = LEFT;

	processActionRoutine();
}

};
