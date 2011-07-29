/*
 * CBigBubble.cpp
 *
 *  Created on: 29.07.2011
 *      Author: gerstrong
 */

#include "CBigBubble.h"

namespace galaxy {

#define A_BUBBLE_MOVE		0

const int BUBBLE_FLOAT_SPEED = 40;

CBigBubble::CBigBubble(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	setupGalaxyObjectOnMap(0x36EC, A_BUBBLE_MOVE);
	m_hDir = NONE;
	m_vDir = UP;
	honorPriority = false;
	solid = false;
}

void CBigBubble::process()
{
	moveUp(BUBBLE_FLOAT_SPEED);

	processActionRoutine();
}

} /* namespace galaxy */
