/*
 * CPoisonSlug.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */

#include "CPoisonSlug.h"

namespace galaxy {

CPoisonSlug::CPoisonSlug(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	m_ActionBaseOffset = 0x2012;
	setActionForce(A_SLUG_MOVE);
}

void CPoisonSlug::process()
{
	processActionRoutine();
}

}
