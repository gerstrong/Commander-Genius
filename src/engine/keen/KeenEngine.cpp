#include <base/GsEvent.h>
#include <base/GsLogging.h>
#include <widgets/GsMenuController.h>

#include "KeenEngine.h"
#include "../CGameLauncher.h"
#include <fileio/ResourceMgmt.h>
#include "fileio/CSaveGameController.h"
#include "fileio/KeenFiles.h"

#include "sdl/audio/music/CMusic.h"
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>



void KeenEngine::switchToGamePlayMode()
{
    const int episode = g_pBehaviorEngine->getEpisode();

    auto &numPlayers = g_pBehaviorEngine->mPlayers;

    // If you get here, you always have at least one player
    if(numPlayers <= 0)
        numPlayers = 1;

    std::string DataDirectory = gKeenFiles.gameDir;
    gEventManager.add( new GMSwitchToPlayGameMode( episode, numPlayers, DataDirectory ) );
}



void KeenEngine::start()
{
    int version;

    CExeFile &ExeFile = gKeenFiles.exeFile;

    version = ExeFile.getEXEVersion();

    gLogging.ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", mEp, version/100, version%100);

    if( ExeFile.getHeaderData() == NULL)
    {
        gLogging.textOut(RED, "KeenEngine::loadResources: Could not load data from the EXE File<br>");
        gEventManager.add(new GMSwitchToGameLauncher() );
        return;
    }

    gKeenFiles.setupFilenames(mEp);

    g_pBehaviorEngine->setEpisode(mEp);

    // Load the Resources
    loadResources( LOADALL );
}

void KeenEngine::pumpEvent(const CEvent *evPtr)
{
    GameEngine::pumpEvent(evPtr);

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

    if(g_pMusicPlayer->active() && !g_pMusicPlayer->playing())
    {
       g_pMusicPlayer->play();
    }

    // Did the player press the quit/back button
    if( gInput.getPressedCommand(IC_BACK) )
    {
        if( gMenuController.empty() ) // If no menu is open, open the main menu
        {
            openMainMenu();
        }
    }
}



