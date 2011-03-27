/*
 * CGameControl.cpp
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include "CGameControl.h"
#include "CResourceLoader.h"
#include "fileio/CExeFile.h"
#include "fileio/CPatcher.h"
#include "fileio.h"
#include "CLogFile.h"
#include "sdl/sound/CSound.h"

#include "core/CGameLauncherMenu.h"
#include "core/CGamePassiveMode.h"
#include "core/CGamePlayMode.h"

#include "arguments.h"

CGameControl::CGameControl(bool &firsttime) :
m_firsttime(firsttime),
m_show_finale(false)
{}

////
// Initialization Routine
////
bool CGameControl::init(int argc, char *argv[])
{
	bool ok = true;
	std::string argument;
	argument = getArgument( argc, argv, "-game" );

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	EventContainer.add( new GMSwitchToGameLauncher() );

	// Check if some arguments were given.
	/*if(argument != "")
	{
		// Get the game number according to the created menu list.
		int chosengame = 0;
		std::string buf = argument.substr(strlen("-game"));
		chosengame = atoi(buf)-1;

		if(chosengame >= 0)
		{
			// Tell the
			mp_GameMode->mp_GameLauncher->setChosenGame(chosengame);

			// Now check, if a level was also passed as parameter
			argument = getArgument( argc, argv, "-level" );
			buf = argument.substr(strlen("-level"));

			//m_startLevel = atoi(buf);
			//m_Numplayers = 1;
		}
	}*/

	return ok;
}

////
// Process Routine
////
// This function is run every time, the Timer says so, through.
void CGameControl::process()
{
	// process any triggered Game Control related event
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	if( GMSwitchToGameLauncher* p_Launcher = EventContainer.occurredEvent<GMSwitchToGameLauncher>() )
	{
		mp_GameMode = new CGameLauncherMenu( m_firsttime, p_Launcher->m_ChosenGame, p_Launcher->m_StartLevel );
		mp_GameMode->init();
		EventContainer.pop_Event();
	}
	else if( GMSwitchToPassiveMode* p_Passive = EventContainer.occurredEvent<GMSwitchToPassiveMode>() )
	{
		mp_GameMode = new CGamePassiveMode( p_Passive->m_DataDirectory, p_Passive->m_Episode );
		mp_GameMode->init();
		EventContainer.pop_Event();
	}
	else if( GMSwitchToPlayGameMode* p_PlayGame = EventContainer.occurredEvent<GMSwitchToPlayGameMode>() )
	{
		mp_GameMode = new CGamePlayMode( p_PlayGame->m_Episode, p_PlayGame->m_Numplayers,
										p_PlayGame->m_Difficulty, p_PlayGame->m_DataDirectory,
										p_PlayGame->m_SavedGame);
		mp_GameMode->init();
		EventContainer.pop_Event();
	}
	else if( EventContainer.occurredEvent<GMQuit>() )
	{
		mp_GameMode.tryDeleteData();
		EventContainer.pop_Event();
		return;
	}

	// Process the game control object
	mp_GameMode->process();
}
