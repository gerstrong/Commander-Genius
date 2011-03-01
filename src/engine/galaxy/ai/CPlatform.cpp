/*
 * CPlatform.cpp
 *
 *  Created on: 23.02.2011
 *      Author: gerhard
 */

#include "CPlatform.h"
#include "CPlayerLevel.h"
#include <iostream>

namespace galaxy {

CPlatform::CPlatform(CMap *pmap, Uint32 x, Uint32 y, direction_t dir,
		std::vector<CObject*>& ObjectPtrs) :
CObject(pmap, x, y, OBJ_PLATFORM)
{
	m_hDir = NONE;
	m_vDir = NONE;

	m_ActionBaseOffset = 0x316A;
	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();
	calcBouncingBoxes();
}

void CPlatform::process()
{
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
	}
}

}
