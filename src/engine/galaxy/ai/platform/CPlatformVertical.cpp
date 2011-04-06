/*
 * CPlatformVertical.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformVertical.h"

namespace galaxy {

CPlatformVertical::CPlatformVertical(CMap *pmap, const Uint32 x, const Uint32 y,
		std::vector<CObject*>& ObjectPtrs) :
CPlatform(pmap, x, y, ObjectPtrs)
{
	// TODO Auto-generated constructor stub

}

void CPlatformVertical::process()
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
