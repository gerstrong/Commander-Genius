#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <base/GsApp.h>
#include <base/utils/StringUtils.h>
#include <base/CInput.h>

#include <widgets/GsMenuController.h>
#include <base/GsArguments.h>

#include "engine/core/CResourceLoader.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/menu/MainMenu.h"
#include "engine/core/menu/SelectionMenu.h"
#include "engine/core/menu/ControlSettings.h"
#include "engine/core/menu/CHelpMenu.h"
#include "engine/CGameLauncher.h"
#include "fileio/CPatcher.h"
#include "fileio/CSaveGameController.h"
#include "engine/core/CMessages.h"
#include "audio/Audio.h"
#include "VorticonEngine.h"

#include "CHelp.h"
#include "CStory.h"
#include "COrderingInfo.h"
#include "CAbout.h"
#include "CCredits.h"
#include "CPreviews.h"

#include "CPassiveVort.h"
#include "CAudioVorticon.h"

#include "playgame/CPlayGameVorticon.h"

#include <fileio/KeenFiles.h>

#include <algorithm>

namespace vorticon
{


bool setupAudio()
{
    CAudioVorticon *audio = new CAudioVorticon;

    if(audio->loadSoundData(0))
    {
        gAudio.setupSoundData(audio->sndSlotMap, audio);
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
    if( !gMenuController.empty() )
        return;

    gMusicPlayer.pause();

    gEventManager.add( new OpenMenuEvent(
                           new MainMenu(mOpenedGamePlay,
                                        Style::VORTICON) ) );
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
            CPatcher Patcher(ExeFile, gBehaviorEngine.mPatchFname);
            Patcher.process();

            mLoader.setPermilage(100);

            gTimer.setLPS(DEFAULT_LPS_VORTICON);

            gBehaviorEngine.readTeleporterTable(p_exedata);

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (EGALATCH, EGASPRIT, etc.)
                // This will also read the Tile-Properties
                CEGAGraphicsVort graphics(mLoader);
                graphics.loadData(version, p_exedata);
                mLoader.setPermilage(400);
                mLoader.setStyle(PROGRESS_STYLE_BITMAP);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(p_exedata, mEp, false, version);
                Messages.extractGlobalStringsUsingExe();
                mLoader.setPermilage(500);
            }

            if( (mFlags & LOADSND) == LOADSND )
            {
                // Load the sound data
                setupAudio();
                mLoader.setPermilage(800);
            }

            gBehaviorEngine.getPhysicsSettings().loadGameConstants(mEp, p_exedata);
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
    gSaveGameController.setGameDirectory(mDataPath);
    gSaveGameController.setEpisode(mEp);

    mpGameMode.reset( new vorticon::CPassiveVort() );
    mpGameMode->init();

    mOpenedGamePlay = false;
    gMusicPlayer.stop();

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
        //gSaveGameController.convertAllOldFormats();
    }
    else if( const NewGamePlayersEvent* pNewGame = dynamic_cast<const NewGamePlayersEvent*>(evPtr) )
    {
        gBehaviorEngine.setNumPlayers(pNewGame->mSelection);
        gEventManager.add( new OpenMenuEvent(new CDifficultySelection(Style::VORTICON)) );
        return;
    }
    // Control Menu Events
    else if( const OpenControlMenuEvent* ctrlMenu =
             dynamic_cast<const OpenControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        gEventManager.add( new OpenMenuEvent(
                                new CControlsettings(players,
                                                     Style::VORTICON) ) );
    }
    else if( const GMSwitchToPlayGameMode* pPlayGame = dynamic_cast<const GMSwitchToPlayGameMode*>(evPtr) )
    {
        const GMSwitchToPlayGameMode &playGame = *pPlayGame;
        mpGameMode.reset( new CPlayGameVorticon(playGame.m_startlevel) );

        if(!gEventManager.empty())
            gEventManager.clear();

        mpGameMode->init();
        mOpenedGamePlay = true;
        gBehaviorEngine.setPause(false);
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
        gBehaviorEngine.setPause(false);
        gEventManager.add( new CloseAllMenusEvent() );
    }
    else if( dynamic_cast<const OpenMainMenuEvent*>(evPtr) )
    {
        gEventManager.add( new OpenMenuEvent( new MainMenu(mOpenedGamePlay,
                                                           Style::VORTICON) ) );
    }
    else if( const StartInfoSceneEvent *scene =
             dynamic_cast<const StartInfoSceneEvent*>(evPtr) )
    {
        gMenuController.lock(true);
        gMenuController.hide(true);
        mpInfoScene = scene->mpScene;
        mpInfoScene->init();
    }    
    else if( const StartHelpEv *scene =
             dynamic_cast<const StartHelpEv*>(evPtr) )
    {
        gMenuController.lock(true);
        gMenuController.hide(true);

        if(scene->mType ==
           StartHelpEv::Variant::THEGAME)
        {
            mpInfoScene.reset(new CHelp("Game"));
        }
        else if(scene->mType ==
           StartHelpEv::Variant::STORY)
        {
            mpInfoScene.reset(new CStory);
        }
        else if(scene->mType ==
           StartHelpEv::Variant::ORDERING)
        {
            mpInfoScene.reset(new COrderingInfo);
        }
        else if(scene->mType ==
           StartHelpEv::Variant::ABOUT_ID)
        {
            mpInfoScene.reset(new CAbout("ID"));
        }
        else if(scene->mType ==
                StartHelpEv::Variant::ABOUT_CG)
        {
            mpInfoScene.reset(new CAbout("CG"));
        }
        else if(scene->mType ==
                StartHelpEv::Variant::CREDITS)
        {
            mpInfoScene.reset(new CCredits());
        }
        else if(scene->mType ==
                StartHelpEv::Variant::PREVIEWS)
        {
            mpInfoScene.reset(new CPreviews());
        }


        mpInfoScene->init();
    }

}

}
