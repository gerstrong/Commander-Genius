/*
 * CBounder.cpp
 *
 *  Created on: 02.07.2011
 *      Author: gerstrong
 */

#include "CBounder.h"

namespace galaxy
{

enum BOUNDER_ACTION
{
A_BOUNDER_BOUNCE = 0,
A_BOUNDER_MOVE = 2,
A_BOUNDER_ONFLOOR = 4,
A_BOUNDER_STUNNED = 5
};

const int MAX_BOUNCE_BOOST = -115;
const int HOR_SPEED = 40;

CBounder::CBounder(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
CStunnable(pmap, foeID, x, y),
bounceboost(0),
mp_CarriedPlayer(NULL)
{
	mActionMap[A_BOUNDER_BOUNCE] = (void (CStunnable::*)()) &CBounder::processBounce;
	//mActionMap[A_BOUNDER_MOVE] = (void (CStunnable::*)()) &CBounder::;
	//mActionMap[A_BOUNDER_ONFLOOR] = (void (CStunnable::*)()) &CBounder::;
	mActionMap[A_BOUNDER_STUNNED] = &CStunnable::processGettingStunned;

	setupGalaxyObjectOnMap(0x2F12, A_BOUNDER_BOUNCE);
	m_hDir = NONE;

}

void CBounder::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		const int m_py2 = player->getYDownPos();
		const int m_y2 = getYUpPos()+(4<<STC);
		if( m_py2 <= m_y2 && !player->supportedbyobject && !player->m_jumpdownfromobject )
		{
			mp_CarriedPlayer = player;
			player->supportedbyobject = true;
		}
	}

	// Was it a bullet? Than make it stunned.
	if( theObject.exists && dynamic_cast<CBullet*>(&theObject) )
	{
		setAction( A_BOUNDER_STUNNED );
		dead = true;
		theObject.dead = true;
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

void CBounder::movePlatLeft(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->moveLeft(amnt);
	}

	// Now move the platform itself.
	moveLeft(amnt);
}

void CBounder::movePlatRight(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->moveRight(amnt);
	}

	// Now move the platform itself.
	moveRight(amnt);
}

void CBounder::movePlayerUp(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->yinertia = yinertia;
	}
}

void CBounder::movePlayerDown(const int amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
	{
		if(!mp_CarriedPlayer->m_jumpdownfromobject)
			mp_CarriedPlayer->yinertia = yinertia;
	}
}


void CBounder::process()
{
	/*if(yinertia < 0)
		movePlayerUp(-yinertia);*/


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

	// check if someone is still standing on the platform
	if(mp_CarriedPlayer)
	{
		if(!hitdetect(*mp_CarriedPlayer) || mp_CarriedPlayer->blockedu)
		{
			mp_CarriedPlayer->supportedbyobject = false;
			mp_CarriedPlayer->m_jumpdownfromobject = false;
			mp_CarriedPlayer = NULL;
		}
	}

	processActionRoutine();
}


};
