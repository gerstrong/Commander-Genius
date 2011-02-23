/*
 * CPlatform.cpp
 *
 *  Created on: 23.02.2011
 *      Author: gerhard
 */

#include "CPlatform.h"

namespace galaxy {

CPlatform::CPlatform(CMap *pmap, Uint32 x, Uint32 y, direction_t dir) :
CObject(pmap, x, y, OBJ_PLATFORM)
{
	m_hDir = NONE;
	m_vDir = NONE;

	m_ActionBaseOffset = 0x316A;
	setActionForce(A_PLATFORM_MOVE);
}

void CPlatform::process()
{
	processActionRoutine();
}

}
