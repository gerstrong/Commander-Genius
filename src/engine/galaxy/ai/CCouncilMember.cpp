/*
 * CCouncilMember.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */

#include "CCouncilMember.h"
#include "CPlayerLevel.h"

namespace galaxy {

CCouncilMember::CCouncilMember(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	m_ActionBaseOffset = 0x1FB8;
	setActionForce(A_COUNCIL_MEMBER_MOVE);
	setActionSprite();
	calcBouncingBoxes();
}

void CCouncilMember::process()
{
	processActionRoutine();
}

void CCouncilMember::getTouchedBy(CObject &theObject)
{
	if(hitdetect(theObject))
	{
		// When Keen touches the Council Member exit the level and add one to the council list
		if(theObject.m_type == OBJ_PLAYER)
		{
			CPlayerLevel &Player = static_cast<CPlayerLevel&>(theObject);

			CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
			EventContainer.add( new EventExitLevel(mp_Map->getLevel(), true) );
			Player.m_Inventory.Item.m_special.ep4.elders++;
		}
	}

}

}
