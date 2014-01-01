/*
 * GsAppState.cpp
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 */

#include "GsAppState.h"
#include "CResourceLoader.h"
#include "fileio/CExeFile.h"
#include "fileio/CPatcher.h"
#include "fileio.h"
#include "CLogFile.h"
#include "sdl/sound/CSound.h"

#include "common/Menu/CMenuController.h"

#include "core/CGameLauncherMenu.h"
#include "core/mode/CGamePlayMode.h"
#include "core/mode/CGamePassiveMode.h"

#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"

#include "core/CGameMain.h"

//#include "common/CGameLauncher.h"
//#include "common/options.h"
//#include "common/Menu/CMenuController.h"
//#include "engine/CEGAGraphics.h"
//#include "engine/CMessages.h"
//#include "engine/CPassive.h"
//#include "core/mode/CGameMode.h"


#include "arguments.h"


void GsAppStateEventSink::pumpEvent(const CEvent *ev)
{
    mpAppState->pumpEvent(ev);
}


GsAppState::GsAppState(bool &firsttime) :
m_firsttime(firsttime),
m_startGame_no(-1),
m_startLevel(-1),
mSink(this)
{
    g_pBehaviorEngine->EventList().regSink(&mSink);
}

GsAppState::~GsAppState()
{
    g_pBehaviorEngine->EventList().unregSink(&mSink);
}

////
// Initialization Routine
////
bool GsAppState::init(int argc, char *argv[])
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

			// Now check, if a difficulty was chosen, otherwise choose easy
			argument = getArgument( argc, argv, "-diff" );
			if(argument != "")
			{
				buf = argument.substr(strlen("-diff"));
				Difficulty startDifficulty = static_cast<Difficulty>(atoi(buf));
				
				// catch invalid entries
				if(startDifficulty < 0 || startDifficulty > 3)
				{
				  // TODO: Tell here that difficulty is invalid and that CG continues on easy
				  startDifficulty = UNKNOWN;
				}
				
				g_pBehaviorEngine->mDifficulty = startDifficulty;
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



void GsAppState::pumpEvent(const CEvent *evPtr)
{
    // Pump all the events related to the root of AppState    
    if( const GMSwitchToGameLauncher* p_Launcher = dynamic_cast<const GMSwitchToGameLauncher*>(evPtr) )
    {
        g_pSound->unloadSoundData();
        gpMenuController->emptyMenuStack();
        mpEngine.reset(new CGameLauncherMenu( m_firsttime,
                          p_Launcher->m_ChosenGame,
                          p_Launcher->m_StartLevel) );
        mpEngine->init();
    }
    else if( dynamic_cast<const StartMainGameEvent*>(evPtr) )
    {
        mpEngine.reset(new CGameMain(gpMenuController->mOpenedGamePlay));
        mpEngine->init();
    }
    else if( dynamic_cast<const GMQuit*>(evPtr) )
    {
        mpEngine.release();
    }
    else if( const InvokeFunctorEvent *iEv = dynamic_cast<const InvokeFunctorEvent*>(evPtr) )
    {
        (*iEv)();
    }
    else // none of the above, let's see if the children have events to be processed
    {

    }
}


void GsAppState::pollEvents()
{
    if( g_pInput->getExitEvent() )
    {
      mpEngine.release();
      return;
    }
}


////
// Process Routine
////
// This function is run every time, the Timer says so, through.
void GsAppState::ponder()
{    
    pollEvents();

	// Process the game control object if no effects are being processed
	if(mpEngine)
        mpEngine->ponder();

	if(g_pGfxEngine->runningEffect())
	{
		if( g_pInput->getPressedAnyCommand() || g_pInput->mouseClicked() )
		{
            g_pGfxEngine->setFXfinished(true);
		}
	}
	
    gpMenuController->ponder();    
}

void GsAppState::render(const float deltaT)
{
    if(mpEngine)
        mpEngine->render(deltaT);

    gpMenuController->render(deltaT);
}
