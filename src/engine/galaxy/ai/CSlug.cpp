/*
 * CSlug.cpp
 *
 *  Created on: 23.02.2011
 *      Author: FCTW
 */

#include "CSlug.h"

namespace galaxy {

CSlug::CSlug(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	m_ActionBaseOffset = 0x2012;
	setActionForce(A_SLUG_MOVE);
}

void CSlug::process()
{
	processActionRoutine();
}

}
