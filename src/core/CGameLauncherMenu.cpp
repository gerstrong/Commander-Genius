/*
 * CGameLauncherMenu.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGameLauncherMenu.h"
#include "CResourceLoader.h"
#include "fileio/CPatcher.h"

#include "CGameMain.h"
#include "engine/vorticon/CEGAGraphicsVort.h"
#include "engine/galaxy/res/CEGAGraphicsGalaxy.h"
#include "engine/CMessages.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/CSaveGameController.h"
#include "sdl/sound/CSound.h"
#include "sdl/CTimer.h"
#include "sdl/CVideoDriver.h"
#include "common/CBehaviorEngine.h"
#include "CLogFile.h"
#include "Debug.h"

CGameLauncherMenu::CGameLauncherMenu(bool& first_time, 
				      const int start_game_no, 
				      const int start_level ) :
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
	mp_EGAGraphics.reset( new CEGAGraphics(0, ".") ); // It has to be the local data path where the interpreter is
	if(!mp_EGAGraphics) return false;
	mp_EGAGraphics->loadData();
	return true;
}

void CGameLauncherMenu::init()
{
    // Here it always makes sense to have the mouse cursor active
    SDL_ShowCursor(SDL_ENABLE);

    // In some cases especially when another game was running, the scene wasn't cleaned up.
    // We do this here
    SDL_Surface *blit = g_pVideoDriver->getBlitSurface();
    SDL_FillRect( blit, nullptr, SDL_MapRGB(blit->format, 0, 0, 0) );

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
		std::unique_ptr<CGameLauncher>& mp_GameLauncher;

		GamesScan(std::unique_ptr<CGameLauncher>& p_GameLauncher) : mp_GameLauncher(p_GameLauncher) {};
		int handle()
		{
			mp_GameLauncher.reset( new CGameLauncher() );
			if(!mp_GameLauncher->init())
			{
                g_pLogFile->textOut(RED,"No game can be launched, because you are missing game data files.<br>");
				return 0;
			}

			return 1;
		}
	};

	const std::string threadname = "Scanning Game-Directory";
	// He we start the thread for cycling the loading screen
	g_pResourceLoader->setStyle(PROGRESS_STYLE_TEXT);
    if(g_pResourceLoader->RunLoadAction(new GamesScan(mp_GameLauncher), threadname) != 0)
    {
		mp_GameLauncher->setChosenGame(m_start_game_no);
    }

	return;
}


bool CGameLauncherMenu::loadGalaxyResources(const Uint8 flags)
{
    CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;
    int version = ExeFile.getEXEVersion();
    unsigned char *p_exedata = ExeFile.getRawData();
    const int Episode = ExeFile.getEpisode();
    
    g_pResourceLoader->setPermilage(10);

    // Patch the EXE-File-Data directly in the memory.
    CPatcher Patcher(ExeFile, g_pBehaviorEngine->m_is_a_mod);
    Patcher.process();    
    
    mp_GameLauncher->setChosenGame(m_start_game_no);
    
    g_pResourceLoader->setPermilage(50);    
    
    if( (flags & LOADGFX) == LOADGFX )
    {
        // Decode the entire graphics for the game (Only EGAGRAPH.CK?)
        mp_EGAGraphics.reset(new galaxy::CEGAGraphicsGalaxy(ExeFile)); // Path is relative to the data directory
        if(!mp_EGAGraphics)
            return false;

        mp_EGAGraphics->loadData();
        g_pResourceLoader->setPermilage(400);
    }
    
    if( (flags & LOADSTR) == LOADSTR )
    {
        // load the strings.
        CMessages Messages(p_exedata, Episode, version);
        Messages.extractGlobalStrings();
        g_pResourceLoader->setPermilage(450);
    }


    if( (flags & LOADSND) == LOADSND )
    {
        g_pLogFile->ftextOut("Loading audio... <br>");
        // Load the sound data
        g_pSound->loadSoundData();
        g_pResourceLoader->setPermilage(900);
        g_pLogFile->ftextOut("Done loading audio.<br>");
    }
    
    g_pLogFile->ftextOut("Loading game constants...<br>");

    g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(Episode, p_exedata);

    g_pLogFile->ftextOut("Looking for patches...<br>");
    
    // If there are patches left that must be applied later, do it here!
    Patcher.postProcess();

    g_pLogFile->ftextOut("Done loading the resources...<br>");
    
    g_pResourceLoader->setPermilage(1000);
    
    return true;
}


///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool CGameLauncherMenu::loadResources( const std::string& DataDirectory, const int Episode, const Uint8 flags )
{
	int version;
	unsigned char *p_exedata;

	CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;

	version = ExeFile.getEXEVersion();
	p_exedata = ExeFile.getRawData();

	g_pLogFile->ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", Episode, version/100, version%100);
	if( Episode == 1 && version == 134) g_pLogFile->ftextOut("This version of the game is not supported!<br>");

	if( ExeFile.getHeaderData() == NULL)
	{
		g_pLogFile->textOut(RED, "GsAppState::loadResources: Could not load data from the EXE File<br>");
		return false;
	}

	gpResource->setupFilenames(Episode);

	g_pBehaviorEngine->setEpisode(Episode);

	if( Episode == 1 || Episode == 2 || Episode == 3 ) // Vorticon resources
	{
	    	// Patch the EXE-File-Data directly in the memory.
		CPatcher Patcher(ExeFile, g_pBehaviorEngine->m_is_a_mod);
		Patcher.process();

		g_pTimer->setLPS(DEFAULT_LPS_VORTICON);
	    
		g_pBehaviorEngine->readTeleporterTable(p_exedata);

		if( (flags & LOADGFX) == LOADGFX )
		{
			// Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
			// This will also read the Tile-Properties
			mp_EGAGraphics.reset( new vorticon::CEGAGraphicsVort(Episode, DataDirectory) );
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
			g_pSound->loadSoundData();
		}

		g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(Episode, p_exedata);
		
		// If there are patches left that must be apllied later, do it here!
		Patcher.postProcess();		

		return true;
	}
    else if( Episode == 4 || Episode == 5 || Episode == 6 ) // Galaxy resources
    {
        g_pTimer->setLPS(DEFAULT_LPS_GALAXY);

        g_pResourceLoader->setStyle(PROGRESS_STYLE_BAR);
        const std::string threadname = "Loading Keen " + itoa(Episode);

        struct GalaxyDataLoad : public Action
        {
            CGameLauncherMenu &mGlm;
            const Uint8 mFlags;

            GalaxyDataLoad(CGameLauncherMenu &glm, const Uint8 flags) :
                mGlm(glm), mFlags(flags) {}

            int handle()
            {
                mGlm.loadGalaxyResources(mFlags);
                return 1;
            }
        };

        if(g_pResourceLoader->RunLoadAction(new GalaxyDataLoad(*this, flags), threadname) == 0)
        {
            g_pBehaviorEngine->EventList().add( new GMQuit() );
            return false;
        }

        return true;
    }
    return false;
}

void CGameLauncherMenu::pumpEvent(const CEvent *evPtr)
{

}

void CGameLauncherMenu::ponder()
{
	// If the firsttime menu is open, process it
	if(mp_FirstTimeMenu)
	{
        /*mp_FirstTimeMenu->ponder();

        if(mp_FirstTimeMenu->mustClose())
            mp_FirstTimeMenu = nullptr;*/
	}
	else
	{
		// Launch the code of the Startmenu here! The one for choosing the games
        mp_GameLauncher->ponder();
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
						CSaveGameController &savedgames = *gpSaveGameController;
						savedgames.setGameDirectory(DataDirectory);
						savedgames.setEpisode(Episode);
						savedgames.convertAllOldFormats();

						EventContainer.add( new StartMainGameEvent );

						if(m_start_level == -1) // Starts normally
							EventContainer.add( new GMSwitchToPassiveMode(DataDirectory, Episode) );
						else // This happens, when a level was passed as argument when launching CG
							EventContainer.add( new GMSwitchToPlayGameMode(Episode, 1,  
													DataDirectory, 
													m_start_level) );
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


void CGameLauncherMenu::render(const float deltaT)
{
    if(!mp_FirstTimeMenu)
    {
        mp_GameLauncher->render(deltaT);
    }
    else
    {
        mp_FirstTimeMenu->render(deltaT);
    }
}
