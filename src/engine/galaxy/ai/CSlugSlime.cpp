/*
 * CSlugSlime.cpp
 *
 *  Created on: 28.05.2011
 *      Author: gerstrong
 */

#include "CSlugSlime.h"

const int A_SLUGPOISON_NORMAL =	0;
const int A_SLUGPOISON_FADE = 1;

CSlugSlime::CSlugSlime(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	m_ActionBaseOffset = 0x20A8;
	setActionForce(A_SLUGPOISON_NORMAL);
	processActionRoutine();
	performCollisions();
}


void CSlugSlime::process()
{
	processFalling();
	processActionRoutine();
}
