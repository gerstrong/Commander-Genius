/*
 * CPlayGameVorticon.cpp
 *
 *  Created on: 03.10.2009
 *      Author: gerstrong
 *
 */

#include <base/GsTimer.h>
#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <widgets/GsMenuController.h>
#include <base/GsArguments.h>
#include <algorithm>

#include "CPlayGameVorticon.h"
#include <base/audio/Audio.h>
#include "engine/core/mode/Scene.h"
#include "engine/core/menu/MainMenu.h"
#include "engine/core/VGamepads/vgamepadsimple.h"
#include "../CVorticonMapLoader.h"
#include "graphics/GsGraphics.h"
#include "../ai/CTeleporter.h"

#include "../finale/CEndingEp1.h"
#include "../finale/CEndingEp2.h"
#include "../finale/CEndingEp3.h"

#include <graphics/CColorMerge.h>

#include <fileio/KeenFiles.h>


////
// Creation Routine
////
/// \brief CPlayGameVorticon::CPlayGameVorticon
/// \param ExeFile
/// \param level
/// \param SavedGame
///
///
CPlayGameVorticon::
CPlayGameVorticon(const int startlevel) :
CPlayGame(startlevel)
{
    // If no level has been set or is out of bound, set it to map.
    if(m_Level > 100 || m_Level <= 0 )
    {
        m_Level = WORLD_MAP_LEVEL_VORTICON;
    }

    mMap.reset(new CMap());
    m_level_command = (m_Level==WORLD_MAP_LEVEL_VORTICON) ? GOTO_WORLD_MAP : START_LEVEL;

    if(!m_Player.empty())
    {
        m_Player.clear();
    }

    const int numPlayers = gBehaviorEngine.numPlayers();

    m_Player.assign( numPlayers, CPlayer(mpLevelCompleted, mMap, 0) );

    for(int i=0 ; i<numPlayers ; i++ )
    {
        // Put some important Player properties
        auto &player = m_Player[i];
        player.m_index = i;
        player.setDatatoZero();
        player.setupCameraObject();
        player.setSpriteVariantId(i);
        player.setSpecialIdx(i);
        player.mpCamera->attachObject(&player);
        player.mpCamera->allowLead(i);
    }


    // Set the whole completed level list to false
    memset( mpLevelCompleted, false, MAX_LEVELS_VORTICON*sizeof(bool));

    m_showPauseDialog = false;

    if(gBehaviorEngine.mDifficulty==EASY)
    {
        gGraphics.Palette.setdarkness(FADE_DARKNESS_EASY);
    }
    else if(gBehaviorEngine.mDifficulty==NORMAL)
    {
        gGraphics.Palette.setdarkness(FADE_DARKNESS);
    }
    else
    {
        gGraphics.Palette.setdarkness(FADE_DARKNESS_HARD);
    }
}

// Setup all the players, when one level is started
void CPlayGameVorticon::setupPlayers()
{
    m_showKeensLeft=false;

    if(!mpHUDVec.empty())
        mpHUDVec.clear();

    const auto numPlayers = gBehaviorEngine.numPlayers();

    for (auto i=0 ; i<numPlayers ; i++)
    {
        auto &player = m_Player[static_cast<unsigned int>(i)];

        if( m_Level == WORLD_MAP_LEVEL_VORTICON )
        {
            player.m_playingmode = CPlayer::WORLDMAP;
            m_showKeensLeft |= ( player.pdie == PDIE_DEAD );
        }
        else
        {
            player.m_playingmode = CPlayer::LEVELPLAY;
            player.mSpriteIdx = PSTANDFRAME;
            player.solid=true;
        }
        player.dontdraw = false;
        player.pdie = PDIE_NODIE;
        const auto p_idx = player.getPlayerIdx();

        if(player.mpCamera)
        {
            player.mpCamera->allowLead(p_idx);
        }

        // Calibrate Player to the right position, so it won't fall when level starts
        GsSprite &sprite = gGraphics.getSprite(i,PSTANDFRAME);
        player.w = sprite.getWidth()<<STC;
        player.h = sprite.getHeight();
        mMap->m_Dark = false;
        gGraphics.Palette.setdark(mMap->m_Dark);

        // Set the pointers to the map and object data
        player.setMapData(mMap);
        player.exists = true;
        player.solid = true;


        stInventory &inventory = m_Player.at(static_cast<unsigned int>(i)).inventory;

        mpHUDVec.push_back( std::unique_ptr<CHUD>(new CHUD(inventory.score, inventory.lives,
                                                                inventory.charges, i,
                                                                i)) );
    }

}

bool CPlayGameVorticon::init()
{
    // Required to get sprites correctly masked
    gGraphics.optimizeSprites();

    // Create the special merge effect (Fadeout)
    CColorMerge *pColorMergeFX = new CColorMerge(8);
    gEffectController.setupEffect(pColorMergeFX);


    CVorticonMapLoaderWithPlayer MapLoader( mMap, m_Player, mSpriteObjectContainer );
    MapLoader.m_checkpointset = m_checkpointset;
    const int numPlayers = gBehaviorEngine.numPlayers();

    // load level map
    if( !MapLoader.load( m_Episode, m_Level, m_Gamepath ) )
        return false;

    gSaveGameController.setLevel(m_Level);
    mMap->setLevel(m_Level);

    //// If those worked fine, continue the initialization
    // draw level map
    mMap->drawAll();

    // Now Scroll to the position of the player and center him
    mMap->gotoPos( 32, 64 ); // Ensure that the edges are never seen

    setupPlayers();

    // Well, all players are living because they were newly spawn.
    gTimer.ResetSecondsTimer();

    gInput.flushAll();

    // Initialize the AI
    mpObjectAI.reset( new CVorticonSpriteObjectAI(mMap, mSpriteObjectContainer, m_Player,
            numPlayers, m_Episode, m_Level,
            mMap->m_Dark) );

    // Check if Player meets the conditions to show a cutscene. This also happens, when finale of episode is reached
    verifyFinales();

    // When Level starts it's never dark!
    gGraphics.Palette.setdark(false);

    std::string finaleStr = gArgs.getValue("finale");
    std::transform(finaleStr.begin(), finaleStr.end(), finaleStr.begin(), ::tolower);
    if(finaleStr == "on")
    {
        createFinale();
        gArgs.removeTag("finale");
    }

    if(m_showKeensLeft)
    {
      gAudio.playSound(int(GameSound::KEENSLEFT), SoundPlayMode::PLAY_NOW);
    }

    // In the case that we are in Episode 3 last Level, show Mortimer Messages
    if( m_Episode == 3 && m_Level == 16 )
    {
        std::unique_ptr<CMessageBoxVort> msg1(new CMessageBoxVort(gBehaviorEngine.getString("EP3_MORTIMER"),false, true));
        std::unique_ptr<CMessageBoxVort> msg2(new CMessageBoxVort(gBehaviorEngine.getString("EP3_MORTIMER2"),false, true));
        std::unique_ptr<CMessageBoxVort> msg3(new CMessageBoxVort(gBehaviorEngine.getString("EP3_MORTIMER3"),false, true));
        std::unique_ptr<CMessageBoxVort> msg4(new CMessageBoxVort(gBehaviorEngine.getString("EP3_MORTIMER4"),false, true));
        std::unique_ptr<CMessageBoxVort> msg5(new CMessageBoxVort(gBehaviorEngine.getString("EP3_MORTIMER5"),false, true));
        std::unique_ptr<CMessageBoxVort> msg6(new CMessageBoxVort(gBehaviorEngine.getString("EP3_MORTIMER6"),false, true));
        mMessageBoxes.push_back(move(msg1));
        mMessageBoxes.push_back(move(msg2));
        mMessageBoxes.push_back(move(msg3));
        mMessageBoxes.push_back(move(msg4));
        mMessageBoxes.push_back(move(msg5));
        mMessageBoxes.push_back(move(msg6));
        gAudio.playSound(int(GameSound::MORTIMER));
    }

    mCurMusicTrack = gMusicPlayer.getCurTrack();
    gMusicPlayer.play();

    return true;
}

bool CPlayGameVorticon::StatusScreenOpen()
{
    const int numPlayers = gBehaviorEngine.numPlayers();
    for( unsigned short i=0 ; i<numPlayers ; i++ )
    {
        if(m_Player[i].m_showStatusScreen)
        {
            m_Player[i].processStatusScreen();
            return true;
        }
    }

    return false;
}

void CPlayGameVorticon::redrawMap()
{
    mMap->drawAll();
}

void CPlayGameVorticon::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    // Process Related Events.
    if( std::dynamic_pointer_cast<const SaveGameEvent>(evPtr) )
    {
        saveXMLGameState();
        gInput.flushAll();
    }
    else if( std::dynamic_pointer_cast<const ResetScrollSurface>(evPtr) )
    {
        if(mMap)
        {
            mMap->drawAll();
            const auto scroll = mMap->getMainScrollCoords();
            gVideoDriver.updateScrollBuffer( scroll.x, scroll.y );
            return;
        }
    }
    else if( std::dynamic_pointer_cast<const EventEndGamePlay>(evPtr) )
    {
        // The last menu has been removed. Restore back the game status
        gBehaviorEngine.setPause(false);
        gMenuController.clearMenuStack();
        gEventManager.add<GMSwitchToPassiveMode>();
    }
    else if( std::dynamic_pointer_cast<const EventReloadMusic>(evPtr) )
    {
        reloadBgMusic();
        gMusicPlayer.play();
    }

    if(mpObjectAI)
    {
        mpObjectAI->pumpEvent(evPtr);
    }

}



void CPlayGameVorticon::reloadBgMusic()
{
    gMusicPlayer.setIMFLoadTrackCallback(imfMusicTrackloader);
    gMusicPlayer.load(mCurMusicTrack);
}



////
// Process Routine
////

void CPlayGameVorticon::ponder(const float deltaT)
{
    CPlayGame::ponder(deltaT);
    _ponder(deltaT);
}

void CPlayGameVorticon::_ponder(const float deltaT)
{   
#ifdef USE_VIRTUALPAD
    VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());

    if(!vkc)
    {
        gInput.mpVirtPad.reset(new VirtualKeenControl);
        gInput.mpVirtPad->init();
        vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
    }

    assert(vkc);
    vkc->mDPad.invisible = false;
    vkc->mMenuButton.invisible = false;
#endif

    if( !mpFinale && !gMenuController.active() ) // Game is not paused, no messages have to be shown and no menu is open
    {
        auto &mplayer = gMusicPlayer;
        const auto curTrack = mplayer.getCurTrack();
        if(mCurMusicTrack != curTrack)
        {
            reloadBgMusic();
        }

        if(mMessageBoxes.empty() && !StatusScreenOpen())
        {
            // Perform AIs
            if(mpObjectAI)
                mpObjectAI->process();

            if( !gBehaviorEngine.paused() )
            {
              // The following functions must be worldmap dependent
              if( m_Level == WORLD_MAP_LEVEL_VORTICON )
              {
                processOnWorldMap(); // TODO: I think the main loop of that should be here!
              }
              else
              {
                processInLevel(); // TODO: I think the main loop of that should be here!
              }

              if(m_Player.size() > static_cast<unsigned int>(mCamLead) )
              {
                  if(m_Player[static_cast<unsigned int>(mCamLead)].pdie)
                  {
                      const int numPlayers = gBehaviorEngine.numPlayers();
                      for( int i=0 ; i<numPlayers ; i++ )
                      {
                          if(m_Player[i].pdie)
                              cycleCamLead();
                      }
                  }
                  else
                  {
                      for(auto &player : m_Player)
                      {
                          // Process Players' Cameras
                          player.processCamera();
                      }
                  }
              }
            }

            for(auto &player : m_Player)
            {
                // Process Player Events
                player.processEvents();
                if(player.level_done == LEVEL_DONE_WALK)
                {
                    mCurMusicTrack = "-1";
                    gMusicPlayer.stop();
                }
            }


        }
    }

    if(mpFinale) // Finale processing if it is opened
    {
        mpFinale->ponder();

        if( mpFinale->getHasFinished() )
        {
            mpFinale.release();

            if(!m_gameover)
            {
                CHighScores *pHighScores = new CHighScores();
                pHighScores->init();
                collectHighScoreInfo(*pHighScores);
                gEventManager.add(new GMSwitchToPassiveMode());
                gEventManager.add(new StartInfoSceneEvent( pHighScores ));
            }
        }
    }

    // Render the dialogs which are seen when the game is paused
    if( !mMessageBoxes.empty() )
    {
        // Finally draw Dialogs like status screen, game paused, etc.
        processPauseDialogs();
    }


    // Check if Sandwich Menu was clicked
    GsRect<float> rRect(mMenuButtonRect);
    const float w = gVideoDriver.getBlitSurface()->w;
    const float h = gVideoDriver.getBlitSurface()->h;

    rRect.pos.y += gVideoDriver.getVidConfig().mHorizBorders;

    rRect.pos.x /= w;       rRect.pos.y /= h;
    rRect.dim.x /= w;       rRect.dim.y /= h;

    if( checkSandwichMenuClicked(rRect) )
    {
        if( gMenuController.empty() ) // If no menu is open, open the main menu
        {
            gEventManager.add(new OpenMainMenuEvent());
        }
    }


    // Check if we are in gameover mode. If yes, than show the bitmaps and block the FKeys().
    // Only confirmation button is allowed
    if(m_gameover && !mpFinale) // game over mode
    {
        if( mpGameoverBmp )
        {
            if( gInput.getPressedAnyCommand() )
            {
                CHighScores *pHighScores = new CHighScores();
                pHighScores->init();
                collectHighScoreInfo(*pHighScores);
                gEventManager.add(new GMSwitchToPassiveMode());
                gEventManager.add(new StartInfoSceneEvent( pHighScores ));
            }
        }
        else // Bitmap must first be created
        {
            GsBitmap *pBitmap = gGraphics.getBitmapFromStr(0, "GAMEOVER");
            gAudio.playSound(int(GameSound::GAME_OVER), SoundPlayMode::PLAY_NOW);
            mpGameoverBmp.reset( new CEGABitmap( mMap , gVideoDriver.getBlitSurface(), pBitmap) );

            GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

            mpGameoverBmp->setScrPos( (gameRes.dim.x/2) -(pBitmap->width()/2),
                                      (gameRes.dim.y/2) -(pBitmap->height()/2) );
        }
    }
    else // No game over
    {
        // Handle special functional keys for paused game, F1 Help, god mode, all items, etc.
        handleFKeys();
        verifyMultiplayerConsistency();
    }

}

void CPlayGameVorticon::render()
{
    // Check for fading processes if necessary
    if(gGraphics.Palette.in_progress())
    {
        gGraphics.Palette.applyFade();
    }

    // Draw all the Stuff here!
    drawAllElements();

    if( m_Level == WORLD_MAP_LEVEL_VORTICON && m_showKeensLeft )
    {
        showKeensLeft();
    }

    if(m_gameover && !mpFinale) // game over mode
    {
        if( mpGameoverBmp )
        {
            mpGameoverBmp->draw();
        }
    }

    if(gBehaviorEngine.mOptions[GameOption::SANDWICHMENU].value)
    {
        // The position of the sandwich menu depends on the configured blit resolution
        mMenuButtonRect.x = gVideoDriver.getBlitSurface()->w-mMenuButtonRect.w;

        auto menuButtonRectWithBorder = mMenuButtonRect;
        menuButtonRectWithBorder.y += gVideoDriver.getVidConfig().mHorizBorders;

        // Draw the Ingame button for mouse/finger control
        drawMenuInGameButton(menuButtonRectWithBorder);
    }
}

void CPlayGameVorticon::cycleCamLead()
{
    mCamLead++;

    const int numPlayers = gBehaviorEngine.numPlayers();
    if( mCamLead >= numPlayers  )
        mCamLead = 0;
}


void CPlayGameVorticon::handleFKeys()
{
    // CTSpace Cheat
    if ((gInput.getHoldedKey(KC) &&
         gInput.getHoldedKey(KT) &&
         gInput.getHoldedKey(KSPACE)) ||
         gBehaviorEngine.mCheatmode.items)
    {
        gBehaviorEngine.mCheatmode.items = false;
        gInput.flushAll();
        const size_t numPlayers = size_t(gBehaviorEngine.numPlayers());
        for(size_t i=0;i<numPlayers;i++)
        {
            m_Player[i].pfiring = false;
            if (m_Player[i].m_playingmode)
            {
                CPhysicsSettings &Phy = gBehaviorEngine.getPhysicsSettings();

                if(Phy.misc.ctspace_keys)
                {
                    m_Player[i].give_keycard(DOOR_YELLOW);
                    m_Player[i].give_keycard(DOOR_RED);
                    m_Player[i].give_keycard(DOOR_GREEN);
                    m_Player[i].give_keycard(DOOR_BLUE);
                }
                else
                {
                    m_Player[i].take_keycard(DOOR_YELLOW);
                    m_Player[i].take_keycard(DOOR_RED);
                    m_Player[i].take_keycard(DOOR_GREEN);
                    m_Player[i].take_keycard(DOOR_BLUE);
                }

                m_Player[i].inventory.charges = Phy.misc.ctspace_ammo;
                m_Player[i].inventory.HasPogo = true;
                m_Player[i].inventory.lives += 5;

                std::string Text = gBehaviorEngine.getString("CTSPACECHEAT");

                std::unique_ptr<CMessageBoxVort> msg(new CMessageBoxVort(Text));

                mMessageBoxes.push_back( move(msg) );
            }
        }
    }

    // Cycle Cam Code
    if( gInput.getPressedCommand(mCamLead, IC_CAMLEAD) )
    {
        cycleCamLead();
    }

    // GOD cheat -- toggle god mode
    if ( gInput.getHoldedKey(KG) &&
         gInput.getHoldedKey(KO) &&
         gInput.getHoldedKey(KD) )
    {
        std::vector<CPlayer>::iterator it_player = m_Player.begin();
        for( ; it_player != m_Player.end() ; it_player++)
        {
            gBehaviorEngine.mCheatmode.god =
                    !gBehaviorEngine.mCheatmode.god;

            gBehaviorEngine.mCheatmode.jump =
                    !gBehaviorEngine.mCheatmode.jump;

            // If player on map, disable the solid property of the players
            if(m_Level == 80)
            {
                it_player->solid = gBehaviorEngine.mCheatmode.noclipping;
                it_player->noclipping = gBehaviorEngine.mCheatmode.noclipping;
            }

            it_player->performCollisions();
        }

        gAudio.playSound(int(GameSound::GUN_CLICK), SoundPlayMode::PLAY_FORCE);

        // Show a message like in the original game
        std::unique_ptr<CMessageBoxVort> msg(
                    new CMessageBoxVort(
                        gBehaviorEngine.mCheatmode.god ?
                            gBehaviorEngine.getString("GODMODEON") :
                            gBehaviorEngine.getString("GODMODEOFF")));
        mMessageBoxes.push_back(move(msg));
        gInput.flushKeys();
    }

    if(gInput.getPressedKey(KP) && mMessageBoxes.empty())
    {
        gAudio.playSound(int(GameSound::GUN_CLICK), SoundPlayMode::PLAY_FORCE);
        std::unique_ptr<CMessageBoxVort> msg( new CMessageBoxVort("Game Paused") );
        mMessageBoxes.push_back(move(msg));
    }

    if(m_Player.empty())
    {
        return;
    }
}


#define ShareWithOthers(pattern) \
    bool pattern = false;  \
    for(size_t i=0;i<numPlayers;i++)  \
    {  \
        if(m_Player[i].inventory.pattern) \
        {  \
            pattern = true;  \
            break;  \
        } \
    }  \
    if(pattern)  \
    { \
        for(size_t i=0;i<numPlayers;i++)  \
        { \
            m_Player[i].inventory.pattern = true; \
        } \
    }


void CPlayGameVorticon::verifyMultiplayerConsistency()
{
    const size_t numPlayers = size_t(gBehaviorEngine.numPlayers());

    ShareWithOthers(HasPogo);
    ShareWithOthers(HasJoystick);
    ShareWithOthers(HasWiskey);
    ShareWithOthers(HasBattery);
    ShareWithOthers(HasVacuum);

}



// The Ending and mortimer cutscenes for example
void CPlayGameVorticon::verifyFinales()
{
    // first we need to know which Episode we were on
    if(m_Episode == 1)
    {
        bool hasBattery, hasWiskey, hasJoystick, hasVaccum;
        hasBattery = hasWiskey = hasJoystick = hasVaccum = false;

        // Check if one of the Players has the items
        const unsigned int numPlayers = gBehaviorEngine.numPlayers();
        for( size_t i=0 ;i < numPlayers ; i++)
        {
            hasBattery |= m_Player[i].inventory.HasBattery;
            hasWiskey |= m_Player[i].inventory.HasWiskey;
            hasJoystick |= m_Player[i].inventory.HasJoystick;
            hasVaccum |= m_Player[i].inventory.HasVacuum;
        }

        // If they have have the items, we can go home
        if(hasBattery && hasWiskey && hasJoystick && hasVaccum)
            createFinale();
    }
    else if(m_Episode == 2)
    {
        bool allCitiesSaved;
        allCitiesSaved = mpLevelCompleted[4];
        allCitiesSaved &= mpLevelCompleted[6];
        allCitiesSaved &= mpLevelCompleted[7];
        allCitiesSaved &= mpLevelCompleted[9];
        allCitiesSaved &= mpLevelCompleted[11];
        allCitiesSaved &= mpLevelCompleted[13];
        allCitiesSaved &= mpLevelCompleted[15];
        allCitiesSaved &= mpLevelCompleted[16];

        if(allCitiesSaved)
            createFinale();
    }
    else if(m_Episode == 3)
    {
        if(mpLevelCompleted[16] == true) // If this level is completed, Mortimer has been killed!
            createFinale();
    }
}

void CPlayGameVorticon::createFinale()
{
    if(m_Episode == 1)
    {
        mpFinale.reset(new CEndingEp1(mMessageBoxes, mMap, m_Player, m_hideobjects, mSpriteObjectContainer));
    }
    else if(m_Episode == 2)
    {
        mpFinale.reset(new CEndingEp2(mMessageBoxes, mMap, m_Player, mSpriteObjectContainer));
    }
    else if(m_Episode == 3)
    {
        mpFinale.reset(new CEndingEp3(mMessageBoxes, mMap, m_Player, mSpriteObjectContainer));
    }
}

void CPlayGameVorticon::teleportPlayerFromLevel(CPlayer &player, int origx, int origy)
{
    int destx, desty;

    std::unique_ptr<CTeleporter> teleporter(
                new CTeleporter(mMap,
                                m_Player,
                                Uint32(origx),
                                Uint32(origy)) );

    player.beingteleported = true;
    player.solid = false;
    destx = gBehaviorEngine.getTeleporterTableAt(5).x;
    desty = gBehaviorEngine.getTeleporterTableAt(5).y;
    teleporter->solid = false;
    teleporter->direction = TELEPORTING_SCROLL;
    teleporter->destx = destx>>TILE_S;
    teleporter->desty = desty>>TILE_S;
    teleporter->whichplayer = int(player.m_index);
    mSpriteObjectContainer.push_back(move(teleporter));
}

void CPlayGameVorticon::collectHighScoreInfo(CHighScores &highScores)
{
    highScores.fetchScoreTable();

    if(m_Episode == 1)
    {
        bool extra[4];

        extra[0] = m_Player[0].inventory.HasJoystick;
        extra[1] = m_Player[0].inventory.HasBattery;
        extra[2] = m_Player[0].inventory.HasVacuum;
        extra[3] = m_Player[0].inventory.HasWiskey;

        highScores.writeEP1HighScore(m_Player[0].inventory.score, extra);
    }
    else if(m_Episode == 2)
    {
        // episode 2: game is won when all cities are saved
        int saved_cities=0;
        if (mpLevelCompleted[4]) saved_cities++;
        if (mpLevelCompleted[6]) saved_cities++;
        if (mpLevelCompleted[7]) saved_cities++;
        if (mpLevelCompleted[13]) saved_cities++;
        if (mpLevelCompleted[11]) saved_cities++;
        if (mpLevelCompleted[9]) saved_cities++;
        if (mpLevelCompleted[15]) saved_cities++;
        if (mpLevelCompleted[16]) saved_cities++;

        highScores.writeEP2HighScore(int(m_Player[0].inventory.score),
                                     saved_cities);
    }
    else
    {
        highScores.writeHighScoreCommon(int(m_Player[0].inventory.score));
    }
}

// This function draws the objects that need to be seen on the screen
void CPlayGameVorticon::drawObjects()
{
    if(m_hideobjects) return;

    for( auto &obj : mSpriteObjectContainer )
        obj->draw();

    // We draw the Player as last, because we want to see him in front of the other objects
    std::vector<CPlayer>::iterator it_player = m_Player.begin();
    std::vector<CPlayer>::iterator it_end = m_Player.end();
    for (; it_player != it_end ; it_player++)
    {
        if(!it_player->beingteleported)
        {
            it_player->draw();
        }
    }

}

void CPlayGameVorticon::drawAllElements()
{
    // Animate the tiles of the map
    mMap->animateAllTiles();

    // Blit the background
    gVideoDriver.blitScrollSurfaces();

    // Draw all objects to the screen
    drawObjects();

    // RenderShake Effect
    mMap->renderShaking();

    // Draw masked tiles here!
    mMap->_drawForegroundTiles();

    for( size_t i=0 ; i<m_Player.size() ; i++ )
    {
        m_Player[i].drawStatusScreen();
    }

    auto &options = gBehaviorEngine.mOptions;

    if(mpFinale) // Finale processing if it is opened
    {
        mpFinale->render();
    }
    else if(options[GameOption::HUD].value ) // Draw the HUD
    {
        for( auto &hud : mpHUDVec )
        {
            hud->render();
        }
    }



    // Process the dialogs which are seen when the game is paused
    if( !mMessageBoxes.empty() )
    {
        // Finally draw Dialogs like status screen, game paused, etc.
        renderPauseDialogs();
    }


}
////
// Cleanup Routine
////
void CPlayGameVorticon::cleanup()
{
    if(!mSpriteObjectContainer.empty())
        mSpriteObjectContainer.clear();
}
