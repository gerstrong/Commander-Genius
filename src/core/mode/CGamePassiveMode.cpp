/*
 * CGamePassiveMode.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGamePassiveMode.h"
#include "sdl/sound/CSound.h"
#include "engine/vorticon/CPassiveVort.h"
#include "engine/galaxy/CPassive.h"
#include <widgets/CMenuController.h>
#include "core/CGameLauncherMenu.h"
#include "common/Menu/CSelectionMenu.h"


CGamePassiveMode::CGamePassiveMode() :
m_Difficulty(0)
{}

void CGamePassiveMode::init()
{
	// Create mp_PassiveMode object used for the screens while Player is not playing
	const int episode = g_pBehaviorEngine->getEpisode();
	if(episode >= 4)
		mpPassive.reset( new galaxy::CPassiveGalaxy() );
	else
		mpPassive.reset( new vorticon::CPassiveVort() );

	if( mpPassive->init() ) return;

    CEventContainer& EventContainer = gEventManager;
	EventContainer.add( new GMSwitchToGameLauncher(-1, -1) );

}


void CGamePassiveMode::ponder(const float deltaT)
{
    mpPassive->ponder();

	// Process Events

    CEventContainer& EventContainer = gEventManager;

	// check here what the player chose from the menu over the passive mode.
	// NOTE: Demo is not part of playgame anymore!!
	if(mpPassive->getchooseGame())
	{
		// TODO: Some of game resources are still not cleaned up here!		
		EventContainer.add( new GMSwitchToGameLauncher(-1, -1) );
		return;
	}

	// User wants to exit. Called from the PassiveMode
	if(mpPassive->getExitEvent())
	{
		EventContainer.add( new GMQuit() );
	}
}

void CGamePassiveMode::render()
{
    mpPassive->render();
}
