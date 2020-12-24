#include <base/GsEvent.h>
#include <base/GsLogging.h>
#include <widgets/GsMenuController.h>

#include "KeenEngine.h"
#include "../CGameLauncher.h"
#include <fileio/ResourceMgmt.h>
#include "fileio/CSaveGameController.h"
#include "fileio/KeenFiles.h"

#include <base/audio/music/CMusic.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>


KeenEngine::~KeenEngine()
{
    gMusicPlayer.stop();
}

void KeenEngine::switchToGamePlayMode(const int startLevel)
{
    const int episode = static_cast<int>(gBehaviorEngine.getEpisode());

    std::string DataDirectory = gKeenFiles.gameDir;
    gEventManager.add( new GMSwitchToPlayGameMode( episode,
                                                   DataDirectory,
                                                   startLevel ) );
}



bool KeenEngine::start()
{
    int version;

    CExeFile &ExeFile = gKeenFiles.exeFile;

    if(!ExeFile.isLuaScript())
    {
        version = ExeFile.getEXEVersion();

        gLogging.ftextOut("Commander Keen Episode %d (Version %d.%d) was detected.<br>", mEp, version/100, version%100);

        if( ExeFile.getHeaderData() == nullptr)
        {
            gLogging.textOut(FONTCOLORS::RED, "KeenEngine::loadResources: Could not load data from the EXE File<br>");
            gEventManager.add(new GMSwitchToGameLauncher() );
            return false;
        }

        gBehaviorEngine.setDemo(ExeFile.isDemo());
    }
    else
    {
        gLogging << "Commander Keen Lua based executable detected!<br>";
    }


    gKeenFiles.setupFilenames(mEp);

    if(!loadGamefileNames())
    {
        return false;
    }

    gBehaviorEngine.setEpisode(mEp);

    auto &&eng = gVideoDriver.mpVideoEngine;

    if(!eng)
    {
        gLogging << "Broken Video Engine!!<br>";
        return false;
    }

    // Load the Resources
    if( !loadResources( LOADALL ) )
    {
        return false;
    }

    return true;
}

void KeenEngine::pumpEvent(const CEvent *evPtr)
{
    GameEngine::pumpEvent(evPtr);

    if( const StartNewGameEvent* pStart = dynamic_cast<const StartNewGameEvent*>(evPtr) )
    {
        gBehaviorEngine.mDifficulty = pStart->mDifficulty;
        switchToGamePlayMode(pStart->mStartLevel);
        return;
    }                    
}

void KeenEngine::ponder(const float deltaT)
{
    if(gMenuController.active())
        return;

    GameEngine::ponder(deltaT);

    if(!mpGameMode)
        return;

    if( gMusicPlayer.paused())
    {
        gMusicPlayer.play();
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



