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

CGamePassiveMode::CGamePassiveMode(const std::string& DataDirectory, const int& Episode) :
m_DataDirectory(DataDirectory),
m_Episode(Episode),
m_Endgame(false),
m_Difficulty(0)
{}

void CGamePassiveMode::init()
{
	// Create mp_PassiveMode object used for the screens while Player is not playing
	if(m_Episode >= 4)
		mp_Passive = new galaxy::CPassiveGalaxy();
	else
		mp_Passive = new vorticon::CPassiveVort();

	if( m_Endgame == true )
	{
		m_Endgame = false;
		// TODO: Overload this function for galaxy
		if( mp_Passive->init(mp_Passive->TITLE) ) return;
	}
	else
	{
		if( mp_Passive->init() ) return;
	}

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	EventContainer.add( new GMSwitchToGameLauncher(-1, -1) );
}

void CGamePassiveMode::process()
{
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	mp_Passive->process();

	if(!EventContainer.empty())
	{
		if( StartGameplayEvent* p_Launcher = EventContainer.occurredEvent<StartGameplayEvent>() )
		{
			const int Episode = mp_Passive->getEpisode();
			//const int Numplayers = mp_Passive->getNumPlayers();
			//const int Difficulty = mp_Passive->getDifficulty();
			const int Numplayers = 1;
			const int Difficulty = 1;
			std::string DataDirectory = mp_Passive->getGamePath();
			CSavedGame SavedGame = mp_Passive->getSavedGameBlock();

			EventContainer.pop_Event();

			EventContainer.add( new GMSwitchToPlayGameMode( Episode, Numplayers, Difficulty, DataDirectory, SavedGame ) );
			return;
		}

	}


	// check here what the player chose from the menu over the passive mode.
	// NOTE: Demo is not part of playgame anymore!!
	if(mp_Passive->getchooseGame())
	{
		// TODO: Some of game resources are still not cleaned up here!
		g_pSound->unloadSoundData();
		EventContainer.add( new GMSwitchToGameLauncher(-1, -1) );
		return;
	}

	// User wants to exit. Called from the PassiveMode
	if(mp_Passive->getExitEvent())
	{
		EventContainer.add( new GMQuit() );
	}

}
