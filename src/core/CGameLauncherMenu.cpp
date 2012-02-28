/*
 * CGameLauncherMenu.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGameLauncherMenu.h"
#include "CResourceLoader.h"
#include "fileio/CPatcher.h"

#include "engine/vorticon/CEGAGraphicsVort.h"
#include "engine/galaxy/CEGAGraphicsGalaxy.h"
#include "engine/CMessages.h"
#include "sdl/sound/CSound.h"
#include "common/CBehaviorEngine.h"
#include "fileio/CSavedGame.h"
#include "SmartPointer.h"
#include "CLogFile.h"
#include "Debug.h"

CGameLauncherMenu::CGameLauncherMenu(bool& first_time, const int start_game_no, const int start_level) :
mp_GameLauncher(NULL),
m_firsttime(first_time),
m_start_game_no(start_game_no),
m_start_level(start_level)
{}

///
// Here the Resources for the menu are loaded. The file used here must exist in the directory
///
bool CGameLauncherMenu::loadMenuResources()
{
	// Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
	mp_EGAGraphics.tryDeleteData();
	mp_EGAGraphics = new CEGAGraphics(0, "."); // It has to be the local data path where the interpreter is
	if(!mp_EGAGraphics.get()) return false;
	mp_EGAGraphics->loadData();
	return true;
}

void CGameLauncherMenu::init()
{
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	// If game was started for the first time, also open the firsttime dialog with configs.
	if(m_firsttime)
	{
		m_firsttime = false;
		//mp_FirstTimeMenu = new CProfilesMenu(DLG_THEME_RED);
	}

	// Load the graphics for menu and background.
    // Resources for the main menu
	if(!loadMenuResources())
	{
		g_pLogFile->textOut(RED, "Sorry, but the basic for creating the menu is missing, please reinstall CG with all the data.<br>");
		EventContainer.add( new GMQuit() );
		return;
	}

	struct GamesScan: public Action
	{
		SmartPointer<CGameLauncher>& mp_GameLauncher;

		GamesScan(SmartPointer<CGameLauncher>& p_GameLauncher) : mp_GameLauncher(p_GameLauncher) {};
		int handle()
		{
			mp_GameLauncher = new CGameLauncher();
			if(!mp_GameLauncher->init())
			{
				g_pLogFile->textOut(RED,"The game cannot start, because you are missing game data files.<br>");
				return 0;
			}

			return 1;
		}
	};

	const std::string threadname = "Scanning Game-Directory";
	// He we start the thread for cycling the loading screen
	g_pResourceLoader->setStyle(PROGRESS_STYLE_TEXT);
	if(g_pResourceLoader->RunLoadAction(new GamesScan(mp_GameLauncher), threadname) == 0)
		EventContainer.add( new GMQuit() );
	else
		mp_GameLauncher->setChosenGame(m_start_game_no);

	return;
}

///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool CGameLauncherMenu::loadResources( const std::string& DataDirectory, const int& Episode, const Uint8 flags )
{
	int version;
	unsigned char *p_exedata;
	unsigned char *p_exeheader;

	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;

    version = ExeFile.getEXEVersion();
	p_exedata = ExeFile.getRawData();
	p_exeheader = ExeFile.getHeaderData();

	g_pLogFile->ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", Episode, version/100, version%100);
	if( Episode == 1 && version == 134) g_pLogFile->ftextOut("This version of the game is not supported!<br>");

	if(p_exeheader == NULL)
	{
		g_pLogFile->textOut(RED, "CGameControl::loadResources: Could not load data from the EXE File<br>");
		return false;
	}

	// Patch the EXE-File-Data directly in the memory.
	CPatcher Patcher(ExeFile, g_pBehaviorEngine->m_is_a_mod);
	Patcher.patchMemory();

	g_pBehaviorEngine->setEpisode(Episode);

	if( Episode == 1 || Episode == 2 || Episode == 3 ) // Vorticon resources
	{
		g_pBehaviorEngine->readTeleporterTable(p_exedata);

		if( (flags & LOADGFX) == LOADGFX )
		{
			// Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
			// This will also read the Tile-Properties
			mp_EGAGraphics.tryDeleteData(); // except for the first start of a game this always happens
			mp_EGAGraphics = new vorticon::CEGAGraphicsVort(Episode, DataDirectory);
			if(!mp_EGAGraphics.get())
				return false;

			mp_EGAGraphics->loadData( version, p_exedata );
		}

		if( (flags & LOADSTR) == LOADSTR )
		{
			// load the strings.
			CMessages Messages(p_exedata, Episode, version);
			Messages.extractGlobalStrings();
		}

		if( (flags & LOADSND) == LOADSND )
		{
			// Load the sound data
			g_pSound->loadSoundData(ExeFile);
		}

		g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(Episode, p_exedata);

		return true;
	}
	else if( Episode == 4 || Episode == 5 || Episode == 6 ) // Galaxy resources
	{
		// TODO: Lots of coding
		if( (flags & LOADGFX) == LOADGFX )
		{
			// Decode the entire graphics for the game (Only EGAGRAPH.CK?)
			mp_EGAGraphics.tryDeleteData();

			mp_EGAGraphics = new galaxy::CEGAGraphicsGalaxy(ExeFile); // Path is relative to the data directory
			if(!mp_EGAGraphics.get())
				return false;

			mp_EGAGraphics->loadData();
		}

		if( (flags & LOADSTR) == LOADSTR )
		{
			// load the strings.
			CMessages Messages(p_exedata, Episode, version);
			Messages.extractGlobalStrings();
		}

		if( (flags & LOADSND) == LOADSND )
		{
			// Load the sound data
			g_pSound->loadSoundData(ExeFile);
		}
	}
	return true;
}


void CGameLauncherMenu::process()
{
	// If the firsttime menu is open, process it
	if(!mp_FirstTimeMenu.empty())
	{
		//mp_FirstTimeMenu->processCommon();
		//mp_FirstTimeMenu->processSpecific();
		//mp_FirstTimeMenu->postProcess();

		/*if(mp_FirstTimeMenu->mustClose())
			mp_FirstTimeMenu.tryDeleteData();*/
	}
	else
	{
		// Launch the code of the Startmenu here! The one for choosing the games
		mp_GameLauncher->process();
		m_start_game_no = mp_GameLauncher->getChosengame();

		if( m_start_game_no >= 0 ) // Means a game has been selected
		{
			//// Game has been chosen. Launch it!
			// Get the path were to Launch the game
			const std::string DataDirectory = mp_GameLauncher->getDirectory( m_start_game_no );

			// We have to check which Episode will be used
			const int Episode = mp_GameLauncher->getEpisode( m_start_game_no );

			if( Episode > 0 ) // The game has to have a valid episode!
			{
				// Get the EXE-Data of the game and load it into the memory.
				if(!g_pBehaviorEngine->m_ExeFile.readData(Episode, DataDirectory))
				{
					mp_GameLauncher->letchooseagain();
				}
				else
				{
					// Load the Resources
					if( loadResources(DataDirectory, Episode) )
					{
						// Now look if there are any old savegames that need to be converted
						CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
						CSavedGame savedgames;
						savedgames.setGameDirectory(DataDirectory);
						savedgames.setEpisode(Episode);
						savedgames.convertAllOldFormats();

						if(m_start_level == -1) // Starts normally
							EventContainer.add( new GMSwitchToPassiveMode(DataDirectory, Episode) );
						else // This happens, when a level was passed as argument when launching CG
							EventContainer.add( new GMSwitchToPlayGameMode(Episode, 1, 1, DataDirectory, savedgames, m_start_level) );
					}
				}
			}
			else
			{
				mp_GameLauncher->letchooseagain();
				g_pLogFile->textOut(RED,"No Suitable game was detected in this path! Please check its contents!\n");
			}
		}
		else if(mp_GameLauncher->getQuit())
		{
			// User chose "exit". So make CG quit...
			CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
			EventContainer.add( new GMQuit() );
		}
	}
}
