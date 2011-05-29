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
CObject(pmap, x, y, OBJ_NONE),
rescued(false)
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
	if(rescued)
		return;

	if(hitdetect(theObject))
	{
		// When Keen touches the Council Member exit the level and add one to the council list
		if(theObject.m_type == OBJ_PLAYER)
		{
			CPlayerLevel &Player = static_cast<CPlayerLevel&>(theObject);

			CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;


			// TODO: Also we need to play the elder sound and the proper Music to it!
			// TODO: In this part we have to check which level we are and send the proper messages



			std::string elder_text[2];
			elder_text[1] = g_pBehaviorEngine->getString("KEEN_NOSWEAT_TEXT");
			elder_text[2] = g_pBehaviorEngine->getString("ELDERS_TEXT");
			EventContainer.add( new EventSendBitmapDialogMsg(0, elder_text[0], LEFT) );
			EventContainer.add( new EventSendBitmapDialogMsg(1, elder_text[1], RIGHT) );




			EventContainer.add( new EventExitLevel(mp_Map->getLevel(), true) );
			Player.m_Inventory.Item.m_special.ep4.elders++;

			rescued = true;
		}
	}

}

}
