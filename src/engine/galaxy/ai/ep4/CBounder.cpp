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

const int MAX_BOUNCE_BOOST = -115;
const int HOR_SPEED = 40;

CBounder::CBounder(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE),
CStunnable(pmap, x, y),
CPlatform(pmap, x, y, OBJ_NONE),
bounceboost(0)
{
	setupGalaxyObjectOnMap(0x2F12, A_BOUNDER_BOUNCE);
	mp_processState = (void (CStunnable::*)()) &CBounder::processBounce;
	m_hDir = NONE;

}

void CBounder::getTouchedBy(CObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);
	CPlatform::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( theObject.exists && dynamic_cast<CBullet*>(&theObject) )
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
		// TODO: This will depend on the position of the player, when he is on the object
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

		return;
	}

	// If Bounder is falling down and Player stands on it, make him move with him
	if( yinertia > 0 && mp_CarriedPlayer)
	{
		CPlayerLevel *Player = dynamic_cast<CPlayerLevel*>(mp_CarriedPlayer);
		if( !Player->m_jumpdownfromobject )
		{
			if( Player->getActionStatus(A_KEEN_STAND) ||
				Player->getActionStatus(A_KEEN_RUN)  )
			{
				Player->yinertia = yinertia;
			}
		}
	}



	if(m_hDir == LEFT)
	{
		movePlatLeft(HOR_SPEED);
	}
	else if(m_hDir == RIGHT)
	{
		movePlatRight(HOR_SPEED);
	}
}

void CBounder::movePlayerUp(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			//mp_CarriedPlayer->moveUp(amnt);
			mp_CarriedPlayer->yinertia = yinertia;
	}
}

void CBounder::movePlayerDown(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			//mp_CarriedPlayer->moveDown(amnt);
			mp_CarriedPlayer->yinertia = yinertia;
	}
}


void CBounder::process()
{
	if(yinertia < 0)
		movePlayerUp(-yinertia);
	//else if(yinertia > 0)
		//movePlayerDown(10);


	// Bounce
	performCollisions();
	processFalling();


	(this->*mp_processState)();

	if( blockedl )
	{
		m_hDir = RIGHT;
		setAction( A_BOUNDER_MOVE );
	}
	else if(blockedr)
	{
		m_hDir = LEFT;
		setAction( A_BOUNDER_MOVE+1 );
	}

	CPlatform::process();
}

};
