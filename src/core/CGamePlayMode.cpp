/*
 * CGamePlayMode.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGamePlayMode.h"
#include "engine/galaxy/CPlayGameGalaxy.h"
#include "engine/vorticon/playgame/CPlayGameVorticon.h"
#include "graphics/effects/CColorMerge.h"

CGamePlayMode::CGamePlayMode(bool& show_finale, Uint8& episode,
		Uint8& Numplayers, Uint8& Difficulty,
		std::string& DataDirectory) :
CGameMode(episode, Numplayers, Difficulty, DataDirectory),
m_startLevel(0),
m_show_finale(show_finale)
{}

bool CGamePlayMode::init()
{
	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;

	bool ok = true;

	if(m_Episode >= 4)
	{
		if(m_startLevel == WORLD_MAP_LEVEL_VORTICON)
			m_startLevel = WORLD_MAP_LEVEL_GALAXY;
		mp_PlayGame = new galaxy::CPlayGameGalaxy( ExeFile, m_startLevel,
												m_Numplayers, m_Difficulty,
												m_SavedGame);
	}
	else
	{
		if(m_startLevel == WORLD_MAP_LEVEL_GALAXY)
			m_startLevel = WORLD_MAP_LEVEL_VORTICON;
		mp_PlayGame = new CPlayGameVorticon( ExeFile, m_startLevel,
											m_Numplayers, m_Difficulty,
											m_show_finale,
											m_SavedGame);
	}

	m_show_finale = false; // just show it once!!

	if(m_SavedGame.getCommand() == CSavedGame::LOAD)
		ok &= mp_PlayGame->loadGameState();
	else
	{
		// Create the special merge effect (Fadeout)
		CColorMerge *pColorMergeFX = new CColorMerge(8);

		ok &= mp_PlayGame->init();

		g_pGfxEngine->pushEffectPtr(pColorMergeFX);
	}

	if(!ok)
	{
		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
		EventContainer.add( new ChangeMode(GM_PASSIVE));
	}

	return ok;
}

void CGamePlayMode::process()
{
	// The player is playing the game. It also includes scenes like ending
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	mp_PlayGame->process();

	if( mp_PlayGame->getEndGame() )
	{
		m_startLevel = 0;
		m_endgame = true;
		EventContainer.add( new ChangeMode(GM_PASSIVE) );
	}
	/*else if( mp_PlayGame->getStartGame() )
	{ // Start another new game
		m_Numplayers = mp_PlayGame->getNumPlayers();
		m_Difficulty = mp_PlayGame->getDifficulty();

		EventContainer.add( new ChangeMode(GM_PLAYGAME) );
	}*/
	else if( mp_PlayGame->getExitEvent() )
	{
		EventContainer.add( new ChangeMode(GM_QUIT) );
	}
}

CGamePlayMode::~CGamePlayMode() {
	// TODO Auto-generated destructor stub
}
