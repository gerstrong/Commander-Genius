#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <base/GsApp.h>
#include <base/utils/StringUtils.h>
#include <widgets/GsMenuController.h>

#include "engine/core/CResourceLoader.h"
#include "GalaxyEngine.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/CGameLauncher.h"
#include "fileio/CPatcher.h"
#include "fileio/CSaveGameController.h"
#include "engine/core/CMessages.h"
#include "sdl/audio/Audio.h"

#include "CPassive.h"
#include "CPlayGameGalaxy.h"

#include "menu/MainMenu.h"
#include "menu/SelectionMenu.h"
#include "menu/ControlSettings.h"
#include "res/CAudioGalaxy.h"
#include <base/video/CVideoDriver.h>
#include "fileio/KeenFiles.h"

namespace galaxy
{


bool setupAudio()
{
    const CExeFile &ExeFile = gKeenFiles.exeFile;
    const unsigned int ep = ExeFile.getEpisode();

    CAudioGalaxy *audio = new CAudioGalaxy(ExeFile);

    if(audio->loadSoundData())
    {
        g_pSound->setupSoundData(audio->sndSlotMapGalaxy[ep], audio);
        return true;
    }

    return false;
}

bool loadLevelMusic(const int level)
{
    Uint16 track;


    CExeFile &ExeFile = gKeenFiles.exeFile;
    const int Idx = ExeFile.getEpisode()-4;

    byte* musictable_start = ExeFile.getRawData()+GalaxySongAssignments[Idx];
    memcpy( &track, musictable_start+level*sizeof(Uint16), sizeof(Uint16));

    if(track > 20)
    {
      gLogging.textOut("Sorry, this track is invalid! Please report the developers.");
      return false;
    }

    return g_pMusicPlayer->loadTrack(ExeFile, track);
}


void GalaxyEngine::openMainMenu()
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
bool GalaxyEngine::loadResources( const Uint8 flags )
{
    gLogging.ftextOut("Loading Galaxy Engine...<br>");

    gTimer.setLPS(DEFAULT_LPS_GALAXY);

    mEngineLoader.setStyle(PROGRESS_STYLE_BAR);
    const std::string threadname = "Loading Keen " + itoa(mEp);

    struct GalaxyDataLoad : public Action
    {
        const Uint8 mFlags;
        CResourceLoaderBackground &mLoader;

        GalaxyDataLoad(const Uint8 flags, CResourceLoaderBackground &loader) :
            mFlags(flags),
            mLoader(loader) {}

        int handle()
        {
            CExeFile &ExeFile = gKeenFiles.exeFile;
            int version = ExeFile.getEXEVersion();
            unsigned char *p_exedata = ExeFile.getRawData();
            const int Episode = ExeFile.getEpisode();

            mLoader.setPermilage(10);

            // Patch the EXE-File-Data directly in the memory.
            CPatcher Patcher(ExeFile, g_pBehaviorEngine->mPatchFname);
            Patcher.process();

            mLoader.setPermilage(50);

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (Only EGAGRAPH.CK?)
                CEGAGraphicsGalaxy graphics(ExeFile);
                if( !graphics.loadData() )
                    return 0;

                mLoader.setPermilage(400);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(p_exedata, Episode, version);
                Messages.extractGlobalStrings();
                mLoader.setPermilage(450);
            }


            if( (mFlags & LOADSND) == LOADSND )
            {
                gLogging.ftextOut("Loading audio... <br>");
                // Load the sound data                
                setupAudio();

                mLoader.setPermilage(900);
                gLogging.ftextOut("Done loading audio.<br>");
            }

            gLogging.ftextOut("Loading game constants...<br>");

            g_pBehaviorEngine->getPhysicsSettings().loadGameConstants(Episode, p_exedata);

            gLogging.ftextOut("Looking for patches...<br>");

            // If there are patches left that must be applied later, do it here!
            Patcher.postProcess();

            gLogging.ftextOut("Done loading the resources...<br>");

            mLoader.setPermilage(1000);

            gEventManager.add(new FinishedLoadingResources());

            return 1;
        }
    };

    mEngineLoader.RunLoadActionBackground(new GalaxyDataLoad(flags, mEngineLoader));
    mEngineLoader.start();

    return true;
}


void GalaxyEngine::switchToPassive()
{
    // Now look if there are any old savegames that need to be converted
    CSaveGameController &savedgames = *gpSaveGameController;
    savedgames.setGameDirectory(mDataPath);
    savedgames.setEpisode(mEp);

    mpGameMode.reset( new galaxy::CPassiveGalaxy() );
    mpGameMode->init();

    g_pMusicPlayer->stop();

    mOpenedGamePlay = false;
}


void GalaxyEngine::pumpEvent(const CEvent *evPtr)
{
    KeenEngine::pumpEvent(evPtr);

    if( dynamic_cast<const FinishedLoadingResources*>(evPtr) )
    {
        switchToPassive();
        gpSaveGameController->convertAllOldFormats();
    }
    else if( dynamic_cast<const EventEndGamePlay*>(evPtr) )
    {
        if( dynamic_cast<CPlayGameGalaxy*>(mpGameMode.get()) )
        {
            switchToPassive();
        }
        else
        {
            gEventManager.add(new GMSwitchToGameLauncher);
        }
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
        const GMSwitchToPlayGameMode &playGame = const_cast<GMSwitchToPlayGameMode&>(*pPlayGame);
        mpGameMode.reset( new CPlayGameGalaxy(playGame.m_startlevel) );
        mpGameMode->init();
        mOpenedGamePlay = true;        
        g_pBehaviorEngine->setPause(false);
        gEventManager.add( new CloseAllMenusEvent() );
    }    
    else if( dynamic_cast<const LoadGameEvent*>(evPtr) ) // If GamePlayMode is not running but loading is requested...
    {
        std::unique_ptr<CPlayGameGalaxy> pgGalaxy(new CPlayGameGalaxy(0));
        pgGalaxy->init();
        pgGalaxy->loadGame();
        mpGameMode = std::move(pgGalaxy);
        mOpenedGamePlay = true;
        g_pBehaviorEngine->setPause(false);
        gEventManager.add( new CloseAllMenusEvent() );
    }
    else if( dynamic_cast<const OpenMainMenuEvent*>(evPtr) )
    {
        openMainMenu();
    }
}


}
