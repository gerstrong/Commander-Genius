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
#include "engine/infoscenes/CHighScores.h"
#include "common/Menu/CMenuController.h"
#include "core/CGameLauncherMenu.h"
#include "common/Menu/CSelectionMenu.h"



CGamePassiveMode::CGamePassiveMode() :
m_Endgame(false),
m_Difficulty(0)
{}

void CGamePassiveMode::init()
{
	// Create mp_PassiveMode object used for the screens while Player is not playing
	const int episode = g_pBehaviorEngine->getEpisode();
	if(episode >= 4)
		mpPassive = new galaxy::CPassiveGalaxy();
	else
		mpPassive = new vorticon::CPassiveVort();

	if( m_Endgame == true )
	{
		m_Endgame = false;
		// TODO: Overload this function for galaxy
		if( mpPassive->init(mpPassive->TITLE) ) return;
	}
	else
	{
		if( mpPassive->init() ) return;
	}

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	EventContainer.add( new GMSwitchToGameLauncher(-1, -1) );

}


void CGamePassiveMode::process()
{

	mpPassive->process();

	// Process Events

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	/*if(!EventContainer.empty())
	{


	}*/

	// check here what the player chose from the menu over the passive mode.
	// NOTE: Demo is not part of playgame anymore!!
	if(mpPassive->getchooseGame())
	{
		// TODO: Some of game resources are still not cleaned up here!
		g_pSound->unloadSoundData();
		EventContainer.add( new GMSwitchToGameLauncher(-1, -1) );
		return;
	}

	// User wants to exit. Called from the PassiveMode
	if(mpPassive->getExitEvent())
	{
		EventContainer.add( new GMQuit() );
	}


}
