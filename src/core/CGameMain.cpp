/*
 * CGameMain.cpp
 *
 *  Created on: 29.03.2012
 *      Author: gerstrong
 */

#include "CGameMain.h"
#include "common/CBehaviorEngine.h"
#include "mode/CGamePassiveMode.h"
#include "mode/CGamePlayMode.h"

void CGameMain::init()
{

}


void CGameMain::process()
{
	// process any triggered Game Main related event
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();

	if( !EventContainer.empty() )
	{

		if( GMSwitchToPassiveMode* p_Passive = EventContainer.occurredEvent<GMSwitchToPassiveMode>() )
		{
			mpGameMode = new CGamePassiveMode( p_Passive->m_DataDirectory, p_Passive->m_Episode );
			mpGameMode->init();
			EventContainer.pop_Event();
		}
		else if( GMSwitchToPlayGameMode* p_PlayGame = EventContainer.occurredEvent<GMSwitchToPlayGameMode>() )
		{
			mpGameMode = new CGamePlayMode( p_PlayGame->m_Episode, p_PlayGame->m_Numplayers,
					p_PlayGame->m_Difficulty, p_PlayGame->m_DataDirectory,
					p_PlayGame->m_SavedGame, mShowFinale, p_PlayGame->m_startlevel);
			mpGameMode->init();
			EventContainer.pop_Event();
		}

	}

	// Process the game mode object
	mpGameMode->process();

}
