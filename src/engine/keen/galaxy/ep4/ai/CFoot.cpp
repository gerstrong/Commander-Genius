/*
 * CFoot.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: gerstrong
 */

#include "CFoot.h"
#include "../../common/ai/CPlayerBase.h"
#include "engine/core/CBehaviorEngine.h"

namespace galaxy {

CFoot::CFoot(std::shared_ptr<CMap> pMap, const Uint16 foeID, const int actionID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pMap, foeID, x, y, 0)
{
	setupGalaxyObjectOnMap(actionID, 0);
	xDirection = RIGHT;

	honorPriority = true;

	performCollisions();
	processActionRoutine();
}

void CFoot::getTouchedBy(CSpriteObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->m_Inventory.addAchievementTask("A foot ahead", 1);
        gEventManager.add( new EventExitLevelWithFoot(mpMap->getLevel(), player->getPlayerIdx()) );
        player->m_Inventory.Item.m_gem.clear();
	}
}


} /* namespace galaxy */
