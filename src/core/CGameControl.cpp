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

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CGameControl::CGameControl(bool &firsttime) :
m_firsttime(firsttime),
m_show_finale(false),
m_episode(0)
{}

////
// Initialization Routine
////
bool CGameControl::init(int argc, char *argv[])
{
	bool ok;
	std::string argument;
	argument = getArgument( argc, argv, "-game" );

	mp_GameMode = new CGameLauncherMenu(m_firsttime, m_episode, m_Numplayers, m_Difficulty, m_DataDirectory);
	ok = mp_GameMode->init();

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
	if( ChangeMode* p_mode = EventContainer.occurredEvent<ChangeMode>() )
	{
		delete mp_GameMode;

		if(p_mode->Mode == GM_GAMELAUNCHER)
		{
			mp_GameMode = new CGameLauncherMenu(m_firsttime, m_episode, m_Numplayers, m_Difficulty, m_DataDirectory);
		}
		else if(p_mode->Mode == GM_PASSIVE)
		{
			mp_GameMode = new CGamePassiveMode(m_episode, m_Numplayers, m_Difficulty, m_DataDirectory);
		}
		else if(p_mode->Mode == GM_PLAYGAME)
		{
			mp_GameMode = new CGamePlayMode(m_show_finale, m_episode, m_Numplayers, m_Difficulty, m_DataDirectory);
		}
		else if(p_mode->Mode == GM_QUIT)
		{
			mp_GameMode = NULL;
			return;
		}

		mp_GameMode->init();
		EventContainer.pop_Event();
	}

	// Process the game control object
	mp_GameMode->process();
}

void CGameControl::cleanup()
{
	delete mp_GameMode;
	mp_GameMode = NULL;
}

CGameControl::~CGameControl() {
	cleanup();
}

