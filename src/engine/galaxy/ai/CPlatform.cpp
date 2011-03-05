/*
 * CPlatform.cpp
 *
 *  Created on: 23.02.2011
 *      Author: gerhard
 */

#include "CPlatform.h"
#include "CPlayerLevel.h"
#include <iostream>

const int MOVE_HORIZ_SPEED = 20;

namespace galaxy {

CPlatform::CPlatform(CMap *pmap, Uint32 x, Uint32 y, direction_t dir,
		std::vector<CObject*>& ObjectPtrs) :
CObject(pmap, x, y, OBJ_PLATFORM)
{
	m_hDir = RIGHT;
	m_vDir = NONE;

	m_ActionBaseOffset = 0x316A;
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBouncingBoxes();
}

void CPlatform::process()
{
	Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
	if(object == 31 )
	{
		m_hDir = (m_hDir == RIGHT) ? LEFT : RIGHT;
	}

	if(m_hDir == RIGHT)
		moveRight(MOVE_HORIZ_SPEED);
	else
		moveLeft(MOVE_HORIZ_SPEED);

	processActionRoutine();
}

void CPlatform::getTouchedBy(CObject &theObject)
{
	if(hitdetect(theObject))
	{
		if(theObject.m_type != OBJ_PLAYER)
			return;

		CPlayerLevel *pPlayer = (CPlayerLevel*)(&theObject);

		std::cout << "\nWHOA!  Stay off my platformz, yo.\n";

		pPlayer->supportedbyobject = true;
	}
}

}
