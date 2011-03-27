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

CGamePassiveMode::CGamePassiveMode(Uint8& episode, Uint8& Numplayers,
		Uint8& Difficulty, std::string& DataDirectory) :
CGameMode(episode, Numplayers, Difficulty, DataDirectory)
{}

bool CGamePassiveMode::init()
{
	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;

	// Create mp_PassiveMode object used for the screens while Player is not playing
	if(m_Episode >= 4)
		mp_PassiveMode = new galaxy::CPassiveGalaxy( ExeFile, m_SavedGame, m_Difficulty );
	else
		mp_PassiveMode = new vorticon::CPassiveVort( ExeFile, m_SavedGame, m_Difficulty );

	if( m_endgame == true )
	{
		m_endgame = false;
		// TODO: Overload this function for galaxy
		if( mp_PassiveMode->init(mp_PassiveMode->TITLE) ) return true;
	}
	else
	{
		if( mp_PassiveMode->init() ) return true;
	}

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	EventContainer.add( new ChangeMode(GM_GAMELAUNCHER) );

	return false;
}

void CGamePassiveMode::process()
{
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	if(mp_PlayGame != NULL)
	{
		if( mp_PlayGame->getEndGame() )
		{
			EventContainer.add( new ChangeMode(GM_PLAYGAME) );
		}
	}
	mp_PassiveMode->process();

	// check here what the player chose from the menu over the passive mode.
	// NOTE: Demo is not part of playgame anymore!!
	if(mp_PassiveMode->getchooseGame())
	{
		// TODO: Some of game resources are still not cleaned up here!
		g_pSound->unloadSoundData();

		EventContainer.add( new ChangeMode(GM_GAMELAUNCHER) );

		return;
	}

	if(mp_PassiveMode->mustStartGame())
	{
		m_Episode = mp_PassiveMode->getEpisode();
		m_Numplayers = mp_PassiveMode->getNumPlayers();
		m_Difficulty = mp_PassiveMode->getDifficulty();
		m_DataDirectory = mp_PassiveMode->getGamePath();

		EventContainer.add( new ChangeMode(GM_PLAYGAME) );
		return;
	}

	// User wants to exit. Called from the PassiveMode
	if(mp_PassiveMode->getExitEvent())
	{
		EventContainer.add( new ChangeMode(GM_QUIT) );
	}

}
