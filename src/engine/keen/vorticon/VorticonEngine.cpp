#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <base/GsApp.h>
#include <base/utils/StringUtils.h>

#include <widgets/GsMenuController.h>
#include <base/GsArguments.h>

#include "engine/core/CResourceLoader.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/CGameLauncher.h"
#include "fileio/CPatcher.h"
#include "fileio/CSaveGameController.h"
#include "engine/core/CMessages.h"
#include "sdl/audio/Audio.h"
#include "VorticonEngine.h"

#include "CPassiveVort.h"
#include "CAudioVorticon.h"

#include "menu/CMainMenu.h"
#include "menu/CSelectionMenu.h"
#include "menu/CControlsettings.h"

#include "playgame/CPlayGameVorticon.h"

#include <fileio/KeenFiles.h>

namespace vorticon
{


bool setupAudio()
{
    const CExeFile &ExeFile = gKeenFiles.exeFile;

    CAudioVorticon *audio = new CAudioVorticon(ExeFile);

    if(audio->loadSoundData())
    {
        g_pSound->setupSoundData(audio->sndSlotMap, audio);
        return true;
    }

    return false;
}

VorticonEngine::VorticonEngine(const bool openedGamePlay,
               const int ep,
               const std::string &dataPath) :
KeenEngine(openedGamePlay, ep, dataPath),
mEp(ep)
{}


void VorticonEngine::ponder(const float deltaT)
{
    if( mpInfoScene )
    {
        mpInfoScene->ponder();
        if( mpInfoScene->destroyed() )
        {
            mpInfoScene->teardown();
            mpInfoScene = nullptr;
            gInput.flushAll();
            gMenuController.lock(false);
            gMenuController.hide(false);
        }
    }
    else
    {
        KeenEngine::ponder(deltaT);
    }
}

void VorticonEngine::render()
{
    if(mpInfoScene)
    {
        mpInfoScene->render();
    }
    else
    {
        KeenEngine::render();
    }
}


void VorticonEngine::openMainMenu()
{
    // Check if music is playing and pause if it is
    if(g_pMusicPlayer->active())
    {
       g_pMusicPlayer->pause();
    }

    gEventManager.add( new OpenMenuEvent( new MainMenu(mOpenedGamePlay) ) );
}


///
// This is used for loading all the resources of the game the use has chosen.
// It loads graphics, sound and text into the memory
///
bool VorticonEngine::loadResources( const Uint8 flags )
{
    gLogging.ftextOut("Loading Vorticon Engine...<br>");

    mEngineLoader.setStyle(PROGRESS_STYLE_BAR);


    struct VorticonDataLoad : public Action
    {
        const int mEp;
        const Uint8 mFlags;
        const std::string &mDataPath;
        CResourceLoaderBackground &mLoader;

        VorticonDataLoad(const int ep,
                         const Uint8 flags,
                         const std::string &datapath,
                         CResourceLoaderBackground &loader) :
            mEp(ep),
            mFlags(flags),
            mDataPath(datapath),
            mLoader(loader) {}

        int handle()
        {
            CExeFile &ExeFile = gKeenFiles.exeFile;
            int version = ExeFile.getEXEVersion();
            unsigned char *p_exedata = ExeFile.getRawData();

            if( mEp == 1 && version == 134)
            {
                gLogging.ftextOut("This version of the game is not supported!<br>");
                return 0;
            }

            // Patch the EXE-File-Data directly in the memory.
            CPatcher Patcher(ExeFile, g_pBehaviorEngine->mPatchFname);
            Patcher.process();

            mLoader.setPermilage(100);

            gTimer.setLPS(DEFAULT_LPS_VORTICON);

            g_pBehaviorEngine->readTeleporterTable(p_exedata);

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
                // This will also read the Tile-Properties
                CEGAGraphicsVort graphics(mEp, mDataPath);
                graphics.loadData(version, p_exedata);
                mLoader.setPermilage(400);
                mLoader.setStyle(PROGRESS_STYLE_BITMAP);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(p_exedata, mEp, version);
                Messages.extractGlobalStrings();
                mLoader.setPermilage(500);
            }

            if( (mFlags & LOADSND) == LOADSND )
            {
                // Load the sound data
                setupAudio();
                mLoader.setPermilage(800);
            }

            g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(mEp, p_exedata);
            mLoader.setPermilage(900);

            // If there are patches left that must be apllied later, do it here!
            Patcher.postProcess();

            mLoader.setPermilage(1000);

            gEventManager.add(new FinishedLoadingResources());
            return 1;
        }
    };

    mEngineLoader.RunLoadActionBackground(new VorticonDataLoad(mEp, flags, mDataPath, mEngineLoader));
    mEngineLoader.start();

    return true;
}

void VorticonEngine::switchToPassiveMode()
{
    // Now look if there are any old savegames that need to be converted
    CSaveGameController &savedgames = *gpSaveGameController;
    savedgames.setGameDirectory(mDataPath);
    savedgames.setEpisode(mEp);

    mpGameMode.reset( new vorticon::CPassiveVort() );
    mpGameMode->init();

    mOpenedGamePlay = false;
    g_pMusicPlayer->stop();

    std::string finaleStr = gArgs.getValue("finale");
    std::transform(finaleStr.begin(), finaleStr.end(), finaleStr.begin(), ::tolower);
    if(finaleStr == "on")
    {
        gEventManager.add( new StartNewGameEvent(EASY) );
    }
}

void VorticonEngine::pumpEvent(const CEvent *evPtr)
{
    KeenEngine::pumpEvent(evPtr);

    if( dynamic_cast<const FinishedLoadingResources*>(evPtr) )
    {        
        switchToPassiveMode();
        gpSaveGameController->convertAllOldFormats();
    }
    else if( const NewGamePlayersEvent* pNewGame = dynamic_cast<const NewGamePlayersEvent*>(evPtr) )
    {
        g_pBehaviorEngine->mPlayers = pNewGame->mSelection;
        gEventManager.add( new OpenMenuEvent(new CDifficultySelection) );
        return;
    }
    // Control Menu Events
    else if( const OpenMovementControlMenuEvent* ctrlMenu = dynamic_cast<const OpenMovementControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        gEventManager.add( new OpenMenuEvent(
                                new CControlSettingsMovement(players) ) );
    }
    else if( const OpenButtonsControlMenuEvent* ctrlMenu = dynamic_cast<const OpenButtonsControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        gEventManager.add( new OpenMenuEvent(
                                new CControlSettingsButtons(players) ) );
    }
    else if( const OpenControlMenuEvent* ctrlMenu = dynamic_cast<const OpenControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        gEventManager.add( new OpenMenuEvent(
                                new CControlsettings(players) ) );
    }
    else if( const GMSwitchToPlayGameMode* pPlayGame = dynamic_cast<const GMSwitchToPlayGameMode*>(evPtr) )
    {
        const GMSwitchToPlayGameMode &playGame = *pPlayGame;
        mpGameMode.reset( new CPlayGameVorticon(playGame.m_startlevel) );

        if(!gEventManager.empty())
            gEventManager.clear();

        mpGameMode->init();
        mOpenedGamePlay = true;
        g_pBehaviorEngine->setPause(false);
        gEventManager.add( new CloseAllMenusEvent() );        
    }
    else if( dynamic_cast<const GMSwitchToPassiveMode*>(evPtr) )
    {
        mpGameMode.reset( new vorticon::CPassiveVort() );
        mpGameMode->init();
    }
    else if( dynamic_cast<const LoadGameEvent*>(evPtr) ) // If GamePlayMode is not running but loading is requested...
    {
        std::unique_ptr<CPlayGameVorticon> pgVort(new CPlayGameVorticon());
        pgVort->init();
        pgVort->loadGame();
        mpGameMode = std::move(pgVort);
        mOpenedGamePlay = true;
        g_pBehaviorEngine->setPause(false);
        gEventManager.add( new CloseAllMenusEvent() );
    }
    else if( dynamic_cast<const OpenMainMenuEvent*>(evPtr) )
    {
        gEventManager.add( new OpenMenuEvent( new MainMenu(mOpenedGamePlay) ) );
    }
    else if( const StartInfoSceneEvent *scene = dynamic_cast<const StartInfoSceneEvent*>(evPtr) )
    {
        gMenuController.lock(true);
        gMenuController.hide(true);
        mpInfoScene = scene->mpScene;
        mpInfoScene->init();
    }
}

}
