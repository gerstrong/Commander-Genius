/*
 * CPlatformHorizontal.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformHorizontal.h"

namespace galaxy {

CPlatformHorizontal::CPlatformHorizontal(CMap *pmap, const Uint32 x, const Uint32 y,
		std::vector<CObject*>& ObjectPtrs) :
CPlatform(pmap, x, y, ObjectPtrs)
{
	m_hDir = RIGHT;
	m_vDir = NONE;
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBouncingBoxes();
}

const int MOVE_HORIZ_SPEED = 20;

void CPlatformHorizontal::process()
{
	Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());

	// If there is a blocker, change the direction
	if( object == 31 )
	{
		m_hDir = (m_hDir == RIGHT) ? LEFT : RIGHT;
	}

	if(m_hDir == RIGHT && blockedr)
		m_hDir = LEFT;

	if(m_hDir == LEFT && blockedl)
		m_hDir = RIGHT;

	if(m_hDir == RIGHT)
		movePlatRight(MOVE_HORIZ_SPEED);
	else
		movePlatLeft(MOVE_HORIZ_SPEED);

	CPlatform::process();
}

}
