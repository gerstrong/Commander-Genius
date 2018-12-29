#include <base/GsLogging.h>
#include <base/GsTimer.h>
#include <base/GsApp.h>
#include <base/utils/StringUtils.h>
#include <widgets/GsMenuController.h>

#include "GalaxyEngine.h"
#include "engine/core/CResourceLoader.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/menu/MainMenu.h"
#include "engine/CGameLauncher.h"
#include "fileio/CPatcher.h"
#include "fileio/CSaveGameController.h"
#include "engine/core/CMessages.h"
#include "sdl/audio/Audio.h"
#include "graphics/effects/CColorMerge.h"

#include "CPassive.h"
#include "CPlayGameGalaxy.h"

//#include "menu/MainMenu.h"
//#include "menu/SelectionMenu.h"
#include "engine/core/menu/ControlSettings.h"
#include "menu/ComputerWrist.h"
#include "res/CAudioGalaxy.h"
#include <base/video/CVideoDriver.h>
#include <fileio/KeenFiles.h>
#include <base/GsArguments.h>

namespace galaxy
{


const Uint32 GalaxySongAssignments[] =
{
    0x02F206,  // Keen 4
    0x03067A,  // Keen 5
    0x03103E,  // Keen 6
    0x02A872   // Keen 6 Demo
};


bool setupAudio()
{        
    const CExeFile &ExeFile = gKeenFiles.exeFile;
    const auto ep = static_cast<unsigned int>(ExeFile.getEpisode());

    CAudioGalaxy *audio = new CAudioGalaxy();

    if(audio->loadSoundData(0))
    {
        gSound.setupSoundData(audio->sndSlotMapGalaxy[ep], audio);
        return true;
    }

    return false;
}

bool loadLevelMusic(const int level)
{

    CExeFile &ExeFile = gKeenFiles.exeFile;
    const auto episode = ExeFile.getEpisode();
    int Idx = episode-4;

    if(episode == 6 && ExeFile.isDemo())
    {
        Idx = 3;
    }

    const std::string path = gKeenFiles.gameDir;

    std::string levelname = "level";
    if(level < 10) levelname += "0";
    levelname += itoa(level) + ".ck" + itoa(episode);

    if(gMusicPlayer.LoadfromMusicTable(path, levelname))
    {
        return true;
    }

    int track = gMusicPlayer.readTrackNofromMusicTable(path, levelname);

    // Does the file provide some number as track number to use.
    if(track <= -1)
    {
        track = 0;
        byte* rawDataPtr = ExeFile.getRawData();
        byte* musictable_start = rawDataPtr+GalaxySongAssignments[Idx];
        memcpy( &track, musictable_start+level*sizeof(Uint16), sizeof(Uint16));
    }

    if(track > 20)
    {
      gLogging.textOut("Sorry, this track is invalid! Please report the developers.");
      return false;
    }

    return gMusicPlayer.loadTrack(track);
}

void GalaxyEngine::ponder(const float deltaT)
{
    const int ep = gBehaviorEngine.getEpisode();

    if(mpComputerWrist)
    {
        mpComputerWrist->ponder(deltaT);
        return;
    }

    KeenEngine::ponder(deltaT);

    if( gInput.getPressedCommand(IC_HELP) && !gMenuController.empty())
    {
        // Check if music is playing and pause if it is
        gMusicPlayer.pause();

        // Episode 6 for a strange reason does not have the help screen
        if(!mpComputerWrist && (ep!=6))
        {
            gBehaviorEngine.setPause(false);
            gEventManager.add( new CloseAllMenusEvent() );

            mpComputerWrist.reset(new ComputerWrist(ep));
        }
    }

}

void GalaxyEngine::render()
{
    KeenEngine::render();

    if(mpComputerWrist)
    {
        mpComputerWrist->render();
    }
}


void GalaxyEngine::openMainMenu()
{    
    if( mpComputerWrist ) return;

    if( !gMenuController.empty() )  return;


    // Check if music is playing and pause if it is
    gMusicPlayer.pause();

    gEventManager.add( new OpenMenuEvent(
                           new MainMenu(mOpenedGamePlay,
                                        GsControl::GALAXY) ) );

    gEffectController.setupEffect( new CColorMerge(16) );
}



bool GalaxyEngine::loadResources( const Uint8 flags )
{
    gLogging << "Loading Galaxy Engine...<br>";

    gTimer.setLPS(DEFAULT_LPS_GALAXY);

    mEngineLoader.setStyle(PROGRESS_STYLE_BAR);

    struct GalaxyDataLoad : public Action
    {
        const Uint8 mFlags;
        CResourceLoaderBackground &mLoader;

        GalaxyDataLoad(const Uint8 flags, CResourceLoaderBackground &loader) :
            mFlags(flags),
            mLoader(loader) {}

        int handleExeFile()
        {
            CExeFile &ExeFile = gKeenFiles.exeFile;
            int version = ExeFile.getEXEVersion();
            unsigned char *p_exedata = ExeFile.getRawData();
            const int Episode = ExeFile.getEpisode();

            mLoader.setPermilage(10);

            // Patch the EXE-File-Data directly in the memory.
            CPatcher Patcher(ExeFile, gBehaviorEngine.mPatchFname);
            Patcher.process();

            mLoader.setPermilage(50);

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (Only EGAGRAPH.CK?)
                CEGAGraphicsGalaxy graphics;
                if( !graphics.loadData() )
                {
                    gLogging << "Sorry, this graphics file is invalid! Quitting...";
                    gEventManager.add( new GMQuit() );
                    return 0;
                }

                mLoader.setPermilage(400);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(p_exedata, Episode, ExeFile.isDemo(), version);
                Messages.extractGlobalStringsUsingExe();
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

            gBehaviorEngine.getPhysicsSettings().loadGameConstants(Episode, p_exedata);

            gLogging.ftextOut("Looking for patches...<br>");

            // If there are patches left that must be applied later, do it here!
            Patcher.postProcess();

            gLogging.ftextOut("Done loading the resources...<br>");

            mLoader.setPermilage(1000);

            //dumpActionFormatToFile("keen5.act", 425);

            gEventManager.add(new FinishedLoadingResources());

            return 1;
        }

        int handlePythonFile()
        {
            mLoader.setPermilage(50);

            const int episode = gBehaviorEngine.getEpisode();

            if( (mFlags & LOADGFX) == LOADGFX )
            {
                // Decode the entire graphics for the game (Only EGAGRAPH.CK?)
                CEGAGraphicsGalaxy graphics;
                if( !graphics.loadData() )
                {
                    gLogging << "Sorry, this graphics file is invalid! Quitting...";
                    gEventManager.add( new GMQuit() );
                    return 0;
                }

                mLoader.setPermilage(400);
            }

            if( (mFlags & LOADSTR) == LOADSTR )
            {
                // load the strings.
                CMessages Messages(nullptr, episode, false, 0);
                Messages.extractGlobalStringsUsingPython();
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

            gBehaviorEngine.getPhysicsSettings().loadGameConstants(episode, nullptr);

            gLogging.ftextOut("Done loading the resources...<br>");

            mLoader.setPermilage(1000);

            gEventManager.add(new FinishedLoadingResources());

            return 1;
        }

        int handle()
        {
            if(gKeenFiles.exeFile.isPythonScript())
            {
                return handlePythonFile();
            }
            else
            {
                return handleExeFile();
            }
        }
    };

    mEngineLoader.RunLoadActionBackground(new GalaxyDataLoad(flags, mEngineLoader));
    mEngineLoader.start();

    return true;
}


void GalaxyEngine::switchToPassive()
{
    // set the appropiate  Episode and GamePath for save games
    CSaveGameController &savedgames = gSaveGameController;
    savedgames.setGameDirectory(mDataPath);
    savedgames.setEpisode(mEp);

    mpGameMode.reset( new galaxy::CPassiveGalaxy() );
    mpGameMode->init();

    gMusicPlayer.stop();

    mOpenedGamePlay = false;
}

void GalaxyEngine::switchToGameplay(const int startLevel,
                                    const std::vector<int> &spriteVars)
{
    mpGameMode.reset( new CPlayGameGalaxy(startLevel, spriteVars) );
    mpGameMode->init();
    mOpenedGamePlay = true;
    gBehaviorEngine.setPause(false);
    gEventManager.add( new CloseAllMenusEvent() );
}


void GalaxyEngine::pumpEvent(const CEvent *evPtr)
{
    KeenEngine::pumpEvent(evPtr);

    const auto style = GsControl::GALAXY;

    if( dynamic_cast<const FinishedLoadingResources*>(evPtr) )
    {
        const auto argLevel = gArgs.getValue("level");

        // Level as parameter given?
        if(!argLevel.empty())
        {
            const int startLevel = atoi(argLevel.c_str());

            // There always is at least one sprite variant, the original one!
            if(mSpriteVars.empty())
            {
                mSpriteVars.push_back(0);
            }

            switchToGameplay(startLevel, mSpriteVars);
            gEventManager.add(new StartNewGameEvent(EASY, startLevel));
        }
        else
        {
            switchToPassive();
        }
        //gSaveGameController.convertAllOldFormats();
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
    else if( const NewGamePlayersEvent* pNewGame =
             dynamic_cast<const NewGamePlayersEvent*>(evPtr) )
    {
        const auto numPlayerVars = pNewGame->mSelection;
        gBehaviorEngine.setNumPlayers(numPlayerVars);

        // Ensure the Sprite variations are correctly setup
        if(numPlayerVars > 1)
        {
            mSpriteVars.clear();
            for(int i=0 ; i<numPlayerVars ; i++ )
            {
                mSpriteVars.push_back(i);
            }

            gEventManager.add( new OpenMenuEvent(
                                   new CDifficultySelection(
                                       GsControl::Style::GALAXY)) );
        }
        else
        {
            gEventManager.add( new OpenMenuEvent(
                                   new CPlayerSpriteVarSelection(GsControl::Style::GALAXY)) );
        }
        return;
    }        
    else if( const SelectPlayerSpriteVarEvent* pStart =
             dynamic_cast<const SelectPlayerSpriteVarEvent*>(evPtr) )
    {
        mSpriteVars.assign(1, pStart->mSpecialIdx);
        gEventManager.add( new OpenMenuEvent(
                               new CDifficultySelection(GsControl::Style::GALAXY)) );
        return;
    }
    // Control Menu Events
    else if( const OpenMovementControlMenuEvent* ctrlMenu =
             dynamic_cast<const OpenMovementControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        gEventManager.add( new OpenMenuEvent(
                                new CControlSettingsMovement(players,
                                                             GsControl::Style::GALAXY) ) );
    }
    else if( const OpenButtonsControlMenuEvent* ctrlMenu =
             dynamic_cast<const OpenButtonsControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        gEventManager.add( new OpenMenuEvent(
                                new CControlSettingsButtons(players,
                                                            GsControl::Style::GALAXY) ) );
    }
    else if( const OpenControlMenuEvent* ctrlMenu =
             dynamic_cast<const OpenControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        gEventManager.add( new OpenMenuEvent(
                                new CControlsettings(players, style) ) );
    }
    else if( const GMSwitchToPlayGameMode* pPlayGame =
             dynamic_cast<const GMSwitchToPlayGameMode*>(evPtr) )
    {
        const GMSwitchToPlayGameMode &playGame = const_cast<GMSwitchToPlayGameMode&>(*pPlayGame);
        switchToGameplay(playGame.m_startlevel, mSpriteVars);
    }    
    else if( dynamic_cast<const LoadGameEvent*>(evPtr) ) // If GamePlayMode is not running but loading is requested...
    {
        // Ensure the Sprite variations are correctly setup
        mSpriteVars.clear();
        for(int i=0 ; i<gBehaviorEngine.numPlayers() ; i++ )
        {
            mSpriteVars.push_back(i);
        }

        std::unique_ptr<CPlayGameGalaxy> pgGalaxy(new CPlayGameGalaxy(0, mSpriteVars));
        pgGalaxy->init();
        pgGalaxy->loadGame();
        mpGameMode = std::move(pgGalaxy);
        mOpenedGamePlay = true;
        gBehaviorEngine.setPause(false);
        gEventManager.add( new CloseAllMenusEvent() );
        gInput.flushAll();
    }
    else if( dynamic_cast<const OpenMainMenuEvent*>(evPtr) )
    {
        openMainMenu();
    }    
    else if( dynamic_cast<const CloseComputerWrist*>(evPtr) )
    {
        mpComputerWrist = nullptr;
    }

    else if( const auto *ocw = dynamic_cast<const OpenComputerWrist*>(evPtr) )
    {
        CExeFile &ExeFile = gKeenFiles.exeFile;
        const int ep = ExeFile.getEpisode();

        mpComputerWrist.reset(new ComputerWrist(ep, ocw->mSection));
    }

}


}
