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

#include "common/Menu/CMenuController.h"
#include "common/Menu/CMainMenu.h"

#include "CGameMain.h"
#include "CGameLauncherMenu.h"
#include "mode/CGamePlayMode.h"
#include "mode/CGamePassiveMode.h"

#include "sdl/CVideoDriver.h"


#include "arguments.h"

CGameControl::CGameControl(bool &firsttime) :
m_firsttime(firsttime),
m_startGame_no(-1),
m_startLevel(-1)
{}

////
// Initialization Routine
////
bool CGameControl::init(int argc, char *argv[])
{
	bool ok = true;
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	std::string argument;
	argument = getArgument( argc, argv, "-game" );

	// Check if some arguments were given.
	if(argument != "")
	{
		// Get the game number according to the created menu list.
		std::string buf = argument.substr(strlen("-game"));
		int chosengame = atoi(buf)-1;

		if(chosengame >= 0)
		{
			// Tell CG to pass the chosen number of game
			m_startGame_no = chosengame;

			// Now check, if a level was also passed as parameter
			argument = getArgument( argc, argv, "-level" );
			if(argument != "")
			{
				buf = argument.substr(strlen("-level"));
				m_startLevel = atoi(buf);
			}
		}
	}

	// Check if finale cutscenes must be shown
	if(getBooleanArgument( argc, argv, "-finale" ))
	{
		argument = getArgument( argc, argv, "-finale" );
		m_startGame_no = atoi(argument.c_str()+strlen("-finale"))-1;
		m_startLevel = WM_MAP_NUM;
	}

	EventContainer.add( new GMSwitchToGameLauncher(m_startGame_no, m_startLevel) );

	return ok;
}

////
// Process Routine
////
// This function is run every time, the Timer says so, through.
void CGameControl::process()
{
	// process any triggered Game Control related event
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();

	if( !EventContainer.empty() )
	{

		if( GMSwitchToGameLauncher* p_Launcher = EventContainer.occurredEvent<GMSwitchToGameLauncher>() )
		{
			mMenuController.emptyMenuStack();
			mpEngine = new CGameLauncherMenu( m_firsttime, p_Launcher->m_ChosenGame, p_Launcher->m_StartLevel );
			mpEngine->init();
			EventContainer.pop_Event();
		}
		else if( EventContainer.occurredEvent<StartMainGameEvent>() )
		{
			mpEngine = new CGameMain(mMenuController.mOpenedGamePlay);
			mpEngine->init();
			EventContainer.pop_Event();
		}
		else if( EventContainer.occurredEvent<GMQuit>() )
		{
			mpEngine.tryDeleteData();
			EventContainer.pop_Event();

			return;
		}
		else if( InvokeFunctorEvent *iEv = EventContainer.occurredEvent<InvokeFunctorEvent>() )
		{
			(*iEv)();
			EventContainer.pop_Event();

			return;
		}
	}

	// Process the game control object
	mpEngine->process();

	mMenuController.process();
}
