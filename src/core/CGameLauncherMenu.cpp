/*
 * CGameLauncherMenu.cpp
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 */

#include "CGameLauncherMenu.h"
#include "CResourceLoader.h"
#include "fileio/CPatcher.h"

#include "GameEngine.h"

#include "engine/vorticon/VorticonEngine.h"
#include "engine/galaxy/GalaxyEngine.h"

#include "engine/vorticon/CEGAGraphicsVort.h"
#include "engine/galaxy/res/CEGAGraphicsGalaxy.h"
#include "engine/CMessages.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/CSaveGameController.h"
#include "sdl/sound/CSound.h"
#include <lib/base/GsTimer.h>
#include <base/video/CVideoDriver.h>
#include "common/CBehaviorEngine.h"
#include <lib/base/GsLogging.h>
#include <base/GsApp.h>
#include <base/Debug.h>
#include <widgets/CMenuController.h>

CGameLauncherMenu::CGameLauncherMenu(const bool first_time,
				      const int start_game_no, 
				      const int start_level ) :
m_firsttime(first_time),
m_start_game_no(start_game_no),
m_start_level(start_level)
{
    g_pSound->unloadSoundData();
    gMenuController.emptyMenuStack();
}

void CGameLauncherMenu::start()
{
    // Here it always makes sense to have the mouse cursor active
    SDL_ShowCursor(SDL_ENABLE);

    // In some cases especially when another game was running, the scene wasn't cleaned up.
    // We do this here
    SDL_Surface *blit = gVideoDriver.getBlitSurface();
    SDL_FillRect( blit, nullptr, SDL_MapRGB(blit->format, 0, 0, 0) );

	// If game was started for the first time, also open the firsttime dialog with configs.
	if(m_firsttime)
	{
		m_firsttime = false;
		//mp_FirstTimeMenu = new CProfilesMenu(DLG_THEME_RED);
	}

	// Load the graphics for menu and background.
    // Resources for the main menu    
    // This is only for the menu. We only need one fontmap for the list of games and some buttons
    g_pGfxEngine->createEmptyFontmaps(1);
    CFont &Font = g_pGfxEngine->getFont(0);

    Font.loadinternalFont();

	struct GamesScan: public Action
	{
        CGameLauncher &mGameLauncher;

        GamesScan(CGameLauncher &gameLauncher) : mGameLauncher(gameLauncher) {}
		int handle()
		{
            if(!mGameLauncher.init())
			{
                gLogging.textOut(RED,"No game can be launched, because you are missing game data files.<br>");
				return 0;
			}

			return 1;
		}
	};

	const std::string threadname = "Scanning Game-Directory";
	// He we start the thread for cycling the loading screen
	g_pResourceLoader->setStyle(PROGRESS_STYLE_TEXT);
    if(g_pResourceLoader->RunLoadAction(new GamesScan(mGameLauncher), threadname) != 0)
    {
        mGameLauncher.setChosenGame(m_start_game_no);
    }

	return;
}




void CGameLauncherMenu::pumpEvent(const CEvent *evPtr)
{
    mGameLauncher.pumpEvent(evPtr);
}

void CGameLauncherMenu::ponder(const float deltaT)
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
        mGameLauncher.ponder(deltaT);
        m_start_game_no = mGameLauncher.getChosengame();

		if( m_start_game_no >= 0 ) // Means a game has been selected
		{
			//// Game has been chosen. Launch it!
			// Get the path were to Launch the game
            const std::string DataDirectory = mGameLauncher.getDirectory( m_start_game_no );

			// We have to check which Episode will be used
            const int episode = mGameLauncher.getEpisode( m_start_game_no );

            if( episode > 0 ) // The game has to have a valid episode!
			{
				// Get the EXE-Data of the game and load it into the memory.
                if(!g_pBehaviorEngine->m_ExeFile.readData(episode, DataDirectory))
				{
                    mGameLauncher.letchooseagain();
				}
				else
				{
                    if(episode >= 1 && episode <= 3)
                    {
                        gEventManager.add( new StartVorticonEngine(false, episode, DataDirectory) );
                    }
                    else if(episode >= 4 && episode <= 7)
                    {
                        gEventManager.add( new StartGalaxyEngine(false, episode, DataDirectory) );
                    }
                    /*else
                    {
                        // Maybe another Game could be added here?
                    }
                    */
				}
			}
			else
			{
                mGameLauncher.letchooseagain();
				gLogging.textOut(RED,"No Suitable game was detected in this path! Please check its contents!\n");
			}
		}
        else if(mGameLauncher.getQuit())
		{
			// User chose "exit". So make CG quit...
            CEventContainer& EventContainer = gEventManager;
			EventContainer.add( new GMQuit() );
		}
	}
}


void CGameLauncherMenu::render()
{
    if(!mp_FirstTimeMenu)
    {
        mGameLauncher.render();
    }
    else
    {
        mp_FirstTimeMenu->render();
    }
}
