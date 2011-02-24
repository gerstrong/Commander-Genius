/*
 * CCouncilMember.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */

#include "CCouncilMember.h"

namespace galaxy {

CCouncilMember::CCouncilMember(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	m_ActionBaseOffset = 0x1FB8;
	setActionForce(A_COUNCIL_MEMBER_MOVE);
}

void CCouncilMember::process()
{
	processActionRoutine();
}

}
