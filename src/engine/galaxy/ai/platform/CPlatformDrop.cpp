/*
 * CPlatformDrop.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformDrop.h"

namespace galaxy {

CPlatformDrop::CPlatformDrop(CMap *pmap, const Uint32 x, const Uint32 y,
		std::vector<CObject*>& ObjectPtrs) :
CPlatform(pmap, x, y, ObjectPtrs),
m_delay_for_drop(0),
m_drop_speed(0)
{
	m_hDir = RIGHT;
	m_vDir = NONE;
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBouncingBoxes();
}

void CPlatformDrop::process()
{
	if(mp_CarriedPlayer)
	{

	}

	if(m_hDir == RIGHT)
		movePlatRight(MOVE_HORIZ_SPEED);
	else
		movePlatLeft(MOVE_HORIZ_SPEED);

	CPlatform::process();
}

}
