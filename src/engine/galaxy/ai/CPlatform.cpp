/*
 * CPlatform.cpp
 *
 *  Created on: 23.02.2011
 *      Author: Gerstrong and FCTW
 */

#include "CPlatform.h"
#include <iostream>

const int MOVE_HORIZ_SPEED = 20;

namespace galaxy {

CPlatform::CPlatform(CMap *pmap, Uint32 x, Uint32 y, direction_t dir,
		std::vector<CObject*>& ObjectPtrs) :
CObject(pmap, x, y, OBJ_PLATFORM),
mp_CarriedPlayer(NULL)
{
	m_hDir = RIGHT;
	m_vDir = NONE;

	m_ActionBaseOffset = 0x316A;
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBouncingBoxes();
}

void CPlatform::movePlatLeft(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		mp_CarriedPlayer->moveLeft(amnt);

	// Now move the platform itself.
	moveLeft(amnt);
}

void CPlatform::movePlatRight(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		mp_CarriedPlayer->moveRight(amnt);

	// Now move the platform itself.
	moveRight(amnt);
}

void CPlatform::movePlatUp(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		mp_CarriedPlayer->moveUp(amnt);

	// Now move the platform itself.
	moveUp(amnt);
}

void CPlatform::movePlatDown(const int& amnt)
{
	// First move the object on platform if any
	if(mp_CarriedPlayer)
		mp_CarriedPlayer->moveDown(amnt);

	// Now move the platform itself.
	moveDown(amnt);
}


void CPlatform::process()
{
	Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());

	// If there is a blocker, change the direction
	if( object == 31 )
	{
		m_hDir = (m_hDir == RIGHT) ? LEFT : RIGHT;
	}

	// check if someone is still standing on the platform
	if(mp_CarriedPlayer)
		if(!hitdetect(*mp_CarriedPlayer))
			mp_CarriedPlayer = NULL;

	if(m_hDir == RIGHT && blockedr)
		m_hDir = LEFT;

	if(m_hDir == LEFT && blockedl)
		m_hDir = RIGHT;

	if(m_hDir == RIGHT)
		movePlatRight(MOVE_HORIZ_SPEED);
	else
		movePlatLeft(MOVE_HORIZ_SPEED);

	processActionRoutine();
}

void CPlatform::getTouchedBy(CObject &theObject)
{
	if(hitdetect(theObject))
	{
		if(theObject.m_type == OBJ_PLAYER)
		{
			CPlayerLevel &Player = static_cast<CPlayerLevel&>(theObject);

			const int m_py2 = Player.getYDownPos();
			const int m_y2 = getYUpPos()+(4<<STC);
			if( m_py2 <= m_y2 && !Player.supportedbyobject )
			{
				mp_CarriedPlayer = &Player;
				Player.supportedbyobject = true;
			}
		}
	}
}

}
