/*
 * CFoot.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: gerstrong
 */

#include "CFoot.h"
#include "engine/galaxy/ai/CPlayerBase.h"
#include "common/CBehaviorEngine.h"

namespace galaxy {

CFoot::CFoot(CMap *pmap, const Uint16 foeID, const int actionID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y)
{
	setupGalaxyObjectOnMap(actionID, 0);
	xDirection = RIGHT;

	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	m_Pos.y -= (rSprite.getHeight()<<(CSF-STC));
	m_Pos.y -= (1<<CSF);

	honorPriority = true;

	performCollisions();
	processActionRoutine();
}

void CFoot::getTouchedBy(CSpriteObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		// TODO: This is not all. He exits the level but must also transport to the secret level. Add this event.
		g_pBehaviorEngine->m_EventList.add( new EventExitLevelWithFoot(mp_Map->getLevel()) );		
		m_Inventory.Item.m_gem.empty();
	}
}


} /* namespace galaxy */
