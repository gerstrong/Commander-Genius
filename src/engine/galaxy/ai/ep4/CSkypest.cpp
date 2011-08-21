/*
 * CSkypest.cpp
 *
 *  Created on: 21.08.2011
 *      Author: gerstrong
 */

#include "CSkypest.h"

namespace galaxy {

#define A_SKYPEST_FLY		0
#define A_SKYPEST_SQUISHED	2
#define A_SKYPEST_LICKEYES	3


CSkypest::CSkypest(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	setupGalaxyObjectOnMap(0x2486, A_SKYPEST_FLY);
}

void CSkypest::process()
{
	processActionRoutine();
}

} /* namespace galaxy */
