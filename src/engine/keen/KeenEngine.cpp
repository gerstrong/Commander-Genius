#include <base/GsEvent.h>
#include <base/GsLogging.h>
#include <widgets/GsMenuController.h>

#include "common/CGameLauncher.h"
#include "KeenEngine.h"
#include "fileio/ResourceMgmt.h"
#include "fileio/CSaveGameController.h"

#include "sdl/music/CMusic.h"
#include <base/CInput.h>


void KeenEngine::switchToGamePlayMode()
{
    const int episode = g_pBehaviorEngine->getEpisode();

    auto &numPlayers = g_pBehaviorEngine->mPlayers;

    // If you get here, you always have at least one player
    if(numPlayers <= 0)
        numPlayers = 1;

    std::string DataDirectory = g_pBehaviorEngine->m_ExeFile.getDataDirectory();
    gEventManager.add( new GMSwitchToPlayGameMode( episode, numPlayers, DataDirectory ) );
}


void KeenEngine::openMainMenu()
{
    /*if(gMenuController.isLocked())
        return;

    gEventManager.add( new OpenMenuEvent( new CMainMenu(mOpenedGamePlay) ) );
    g_pBehaviorEngine->setPause(true);
    g_pMusicPlayer->pause();*/

    /*
    // Control Menu Events
    if( const OpenMovementControlMenuEvent* ctrlMenu = dynamic_cast<const OpenMovementControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        EventContainer.add( new OpenMenuEvent(
                                new CControlSettingsMovement(players) ) );
    }

    if( const OpenButtonsControlMenuEvent* ctrlMenu = dynamic_cast<const OpenButtonsControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        EventContainer.add( new OpenMenuEvent(
                                new CControlSettingsButtons(players) ) );
    }

    if( const OpenControlMenuEvent* ctrlMenu = dynamic_cast<const OpenControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        EventContainer.add( new OpenMenuEvent(
                                new CControlsettings(players) ) );
    }*/


    // When menu is opened show the cursor
    //SDL_ShowCursor(SDL_ENABLE);
}


void KeenEngine::start()
{
    int version;

    CExeFile &ExeFile = g_pBehaviorEngine->m_ExeFile;

    version = ExeFile.getEXEVersion();

    gLogging.ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", mEp, version/100, version%100);

    if( ExeFile.getHeaderData() == NULL)
    {
        gLogging.textOut(RED, "KeenEngine::loadResources: Could not load data from the EXE File<br>");
        gEventManager.add(new GMSwitchToGameLauncher() );
        return;
    }

    gpResource->setupFilenames(mEp);

    g_pBehaviorEngine->setEpisode(mEp);

    // Load the Resources
    loadResources( LOADALL );
}

void KeenEngine::pumpEvent(const CEvent *evPtr)
{
    GameEngine::pumpEvent(evPtr);

    /*if( dynamic_cast<const FinishedLoadingResources*>(evPtr) )
    {
        // Now look if there are any old savegames that need to be converted
        CSaveGameController &savedgames = *gpSaveGameController;
        savedgames.setGameDirectory(mDataPath);
        savedgames.setEpisode(mEp);
        savedgames.convertAllOldFormats();

        std::unique_ptr<CGamePassiveMode> passive(new CGamePassiveMode());
        mpGameMode = move(passive);
        mpGameMode->init();
        mOpenedGamePlay = false;
        gMenuController.emptyMenuStack();*/

        //if(m_start_level == -1) // Starts normally
            //gEventManager.add( new GMSwitchToPassiveMode(mDataPath, mEp) );
        /*else // This happens, when a level was passed as argument when launching CG
            EventContainer.add( new GMSwitchToPlayGameMode(episode, 1,
                                    DataDirectory,
                                    m_start_level) );*//*
    }*/

    if( const StartNewGameEvent* pStart = dynamic_cast<const StartNewGameEvent*>(evPtr) )
    {
        g_pBehaviorEngine->mDifficulty = pStart->mDifficulty;
        switchToGamePlayMode();
        return;
    }

}

void KeenEngine::ponder(const float deltaT)
{
    if(gMenuController.active())
        return;

    GameEngine::ponder(deltaT);

    //CEventContainer &EventContainer = gEventManager;

    // Did the player press the quit/back button
    if( gInput.getPressedCommand(IC_BACK) )
    {
        if( gMenuController.empty() ) // If no menu is open, open the main menu
        {
            openMainMenu();
        }
        /*else // Close the menu which is open. Might go back if it is a submenu
        {
            EventContainer.add( new CloseMenuEvent() );
        }*/
    }
}



