/*
 * CPlatformVertical.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformVertical.h"

// Vertical platform speed
const int MOVE_VERT_SPEED = 20;

namespace galaxy {

CPlatformVertical::CPlatformVertical(CMap *pmap, const Uint32 x, const Uint32 y) :
CObject(pmap, x, y, OBJ_PLATFORM),
CPlatform(pmap, x, y, OBJ_PLATFORM)
{
	m_hDir = NONE;
	m_vDir = DOWN;
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBouncingBoxes();

}

void CPlatformVertical::process()
{
	Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());

	// If there is a blocker, change the direction
	if( object == 31 )
		m_vDir = (m_vDir == UP) ? DOWN : UP;

	if(m_vDir == UP && blockedu)
		m_vDir = DOWN;
	else if(m_vDir == DOWN && blockedd)
		m_vDir = UP;

	if(m_vDir == UP)
		movePlatUp(MOVE_VERT_SPEED);
	else
		movePlatDown(MOVE_VERT_SPEED);

	CPlatform::process();
}



}
