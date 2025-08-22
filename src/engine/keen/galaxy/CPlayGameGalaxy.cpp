/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <base/interface/StringUtils.h>
#include <base/video/CVideoDriver.h>
#include <widgets/GsMenuController.h>
#include <iostream>

#include <base/interface/property_tree/property_tree.h>

#include "CPlayGameGalaxy.h"
#include "common/CMapLoaderGalaxy.h"
#include "graphics/GsGraphics.h"
#include "common/dialog/CMessageBoxBitmapGalaxy.h"
#include "common/dialog/CMessageBoxSelection.h"

#include <base/audio/Audio.h>
#include <base/audio/music/CMusic.h>
#include <graphics/CColorMerge.h>
#include <graphics/CDimDark.h>
#include "fileio/CSaveGameController.h"

#include "ep4/ai/CInchWorm.h"
#include "common/ai/CPlayerLevel.h"
#include "common/ai/CPlayerWM.h"
#include "engine/core/VGamepads/vgamepadsimple.h"
#include "engine/core/menu/MainMenu.h"

#include <fileio/KeenFiles.h>
#include <base/GsArguments.h>


namespace galaxy
{



CPlayGameGalaxy::CPlayGameGalaxy(const int startlevel,
                                 const std::vector<int> &spriteVars) :
CPlayGame(startlevel),
m_WorldMap( mInventoryVec),
m_LevelPlay( mInventoryVec)
{
    const int numPlayer = gBehaviorEngine.numPlayers();
    mDead.assign(numPlayer, false);
    mGameOver.assign(numPlayer, false);

    mInventoryVec.resize(numPlayer);

    for(int i=0 ; i<numPlayer ; i++)
    {
        mInventoryVec[i].setup(i, spriteVars[i]);
    }

    m_WorldMap.init();

    m_Level = startlevel;

    // If no level has been set or is out of bound, set it to map.
    if(m_Level > 100 || m_Level < 0 )
    {
        m_Level = WORLD_MAP_LEVEL_GALAXY;
    }

    // Create the special merge effect (Fadeout)
    CColorMerge *pColorMergeFX = new CColorMerge(8);
    gEffectController.setupEffect(pColorMergeFX);
}


bool CPlayGameGalaxy::loadXMLGameState()
{
    /// Create tree for reading
    using GsKit::ptree;
    ptree pt;

    CSaveGameController &savedGame = gSaveGameController;
    if(!savedGame.loadXMLTree(pt))
        return false;

    /// Read the nodes and store the data as needed
    ptree &stateNode = pt.get_child("GameState");


    /// Load the Game in the CSavedGame object
    // Get the episode, and difficulty
    m_Episode = stateNode.get<int>("episode");
    gBehaviorEngine.mDifficulty =
            static_cast<Difficulty>(stateNode.get<int>("difficulty", 1));

    // Get number of Players
    const unsigned int numPlayers = stateNode.get<int>("NumPlayer");
    gBehaviorEngine.setNumPlayers(numPlayers);

    if(!mInventoryVec.empty())
        mInventoryVec.clear();


    mInventoryVec.resize(numPlayers);
    mDead.assign(numPlayers, false);
    mGameOver.assign(numPlayers, false);

    unsigned int variant;

    for( auto &node : pt )
    {
        if(node.first == "death")
        {
            ptree &deadNode = node.second;
            const int id = deadNode.get<int>("<xmlattr>.player", 0);
            mDead[id] = deadNode.get<bool>("<xmlattr>.dead", false);
            mGameOver[id] = deadNode.get<bool>("<xmlattr>.gameover", false);
        }

        if(node.first == "Player")
        {
            ptree &playerNode = node.second;
            variant = playerNode.get<int>("<xmlattr>.variant");
            const int idx = playerNode.get<int>("<xmlattr>.id", 0);
            auto &invNode = playerNode.get_child("inventory");
            mInventoryVec[idx].setup(idx, variant);
            mInventoryVec[idx] << invNode;
        }
    }



    bool active;
    ptree &wmNode = stateNode.get_child("WorldMap");
    active = wmNode.get<bool>("<xmlattr>.active", false);
    if(active)
    {
        mWorldMapActive = true;
        m_WorldMap.refreshGameplayMap();
    }
    m_WorldMap << wmNode;

    ptree &levelPlayNode = stateNode.get_child("LevelPlay");
    active = levelPlayNode.get<bool>("<xmlattr>.active");

    if( active )
    {
        mWorldMapActive = false;
        m_LevelPlay << levelPlayNode;
        m_LevelPlay.refreshGameplayMap();
    }

    return true;
}


bool CPlayGameGalaxy::saveXMLGameState()
{
    /// Create tree
    using GsKit::ptree;
    ptree pt;

    /// Create the nodes and store the data as needed
    ptree &stateNode = pt.add("GameState", "");

    /// Save the Game in the CSavedGame object
    // store the episode, level and difficulty
    stateNode.put("episode", int(m_Episode));


    /// Save the Game in the CSavedGame object
    // store the episode, level and difficulty
    stateNode.put("difficulty", gBehaviorEngine.mDifficulty);

    // Save number of Players
    const size_t numPlayers = size_t(gBehaviorEngine.numPlayers());
    stateNode.put("NumPlayer", numPlayers);

    ptree &deadNode = pt.add("death", "");
    for (size_t id = 0 ; id < mDead.size() ; id++)
    {
        deadNode.put("<xmlattr>.player", id);
        deadNode.put("<xmlattr>.dead", mDead[id]);
        deadNode.put("<xmlattr>.gameover", mGameOver[id]);
    }

    for (size_t id=0 ; id<numPlayers ; id++)
    {
        ptree &playerNode = pt.add("Player", "");
        playerNode.put("<xmlattr>.variant", mInventoryVec[id].mSpriteVar);
        playerNode.put("<xmlattr>.id", id);
        auto &invNode = playerNode.put("inventory", "");
        mInventoryVec[id] >> invNode;
    }

    bool active = mWorldMapActive;
    ptree &wmNode = stateNode.add("WorldMap", "");
    wmNode.put("<xmlattr>.active", active);
    m_WorldMap >> wmNode;

    active = !mWorldMapActive;
    ptree &levelPlayNode = stateNode.add("LevelPlay", "");
    levelPlayNode.put("<xmlattr>.active", active);

    if( active )
    {
        m_LevelPlay >> levelPlayNode;
    }

    if( gSaveGameController.saveXMLTree(pt) )
    {
        return true;
    }

    return false;
}




// Setup for the ingame
bool CPlayGameGalaxy::init()
{
    // Required to sprites are correctly masked
    mGameOverTriggered = false;
    gGraphics.optimizeSprites();


    if(m_Level == 0)
    {
        mWorldMapActive = true;
        m_WorldMap.refreshGameplayMap();
        m_WorldMap.loadAndPlayMusic();
    }
    else
    {
        // manually a level has been loaded
        mWorldMapActive = false;
        m_LevelPlay.loadLevel(0, m_Level);
        m_LevelPlay.refreshGameplayMap();
    }

    const int numPlayers = gBehaviorEngine.numPlayers();
    mDead.assign(numPlayers, false);
    mGameOver.assign(numPlayers, false);

    return true;
}


/**
 *	\description This event triggers a MessageBox where you can select multiple items
 *
 *	\param		Message This Text will be shown when the Box is triggered
 *	\param 		OptionStrings The Text to the option which can be selected
 *							  Depending on the size of the
 */
struct EventSendSelectionDialogMsg : CEvent {

    const std::string Message;
    std::list<TextEventMatchOption> Options;

    EventSendSelectionDialogMsg(const std::string& lMsg) :
                                Message(lMsg){}

    void addOption(const std::string& ltext, CEvent *levent)
    {
        TextEventMatchOption NewOption;
        NewOption.text = ltext;
        NewOption.event.reset( levent );
        Options.push_back(NewOption);
    }

    void addOption(const std::string& ltext,
                   const std::function<void()> &f)
    {
        auto *ev = new FunctionToEvent(f);
        addOption(ltext, ev);
    }

};



struct EventRestartLevel : CEvent {};



void CPlayGameGalaxy::looseManagement( const int playerIdx,
                                       const uint16_t levelObj,
                                       const std::string &levelName )
{
    CEventContainer &eventContainer = gEventManager;

    // Check if all players are dead
    bool allDead = true;
    bool allGameOver = true;

    unsigned int nextAliveID = 0;

    while(mDead[nextAliveID])
    {
        nextAliveID++;
    }

    mDead[playerIdx] = true;
    mInventoryVec[playerIdx].Item.m_lifes--;

    const bool playerGameOver = (mInventoryVec[playerIdx].Item.m_lifes<0);

    if(playerGameOver)
    {
        mGameOver[playerIdx] = true;
    }

    for(auto deadIt = mDead.begin() ; deadIt != mDead.end() ; deadIt++ )
    {
        allDead &= (*deadIt);
    }

    for(auto goIt = mGameOver.begin() ; goIt != mGameOver.end() ; goIt++ )
    {
        allGameOver &= (*goIt);
    }


    if(allDead && !allGameOver) // No, but in the level all are dead!
    {
        // Player should see the dialog first.
        gInput.flushAll();

        // Create the Event Selection screen
        std::string loosemsg  = "You didn't make it past\n";
        loosemsg += levelName;
        EventSendSelectionDialogMsg *pdialogevent = new EventSendSelectionDialogMsg(loosemsg);


        if(gSaveGameController.hasQuickloadGame())
        {
            std::string quickloadMsg = "Quickload!";
            pdialogevent->addOption(quickloadMsg,
                                    []()
                                    {
                                        gSaveGameController.prepareLoadGameQuick();
                                    } );
        }

        pdialogevent->addOption("Try Again", new EventRestartLevel() );

        std::string exitMsg = "Exit to " + gBehaviorEngine.mapLevelName;
        pdialogevent->addOption(exitMsg, new EventExitLevel( levelObj, false, false, playerIdx) );

        eventContainer.add( pdialogevent );

        for(auto deadIt = mDead.begin() ; deadIt != mDead.end() ; deadIt++ )
        {
            (*deadIt) = false;
        }
    }

    // Now let's check for important items
    // If one player dies, the next one should inherit some important stuff,
    // so he can finish the level.
    const unsigned int numDeadPlayers = mDead.size();
    if( numDeadPlayers > 1 && nextAliveID < numDeadPlayers )
    {
        CInventory &next = mInventoryVec[nextAliveID];
        CInventory &dying = mInventoryVec[playerIdx];

        next.fetchImportantStuff(dying);
    }

}


void CPlayGameGalaxy::pumpEvent(const std::shared_ptr<CEvent> &evPtr)
{
    // In this part we will poll all the relevant Events that are important for the
    // Galaxy Main Engine itself. For example, load map, setup world map, show Highscore
    // are some of those events.

    if( std::dynamic_pointer_cast<const SaveGameEvent>(evPtr) )
    {
        saveXMLGameState();
        gInput.flushAll();
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventSendDialog>(evPtr) )
    {
        auto &msgBox = ev->mMsgBox;

        mMessageBoxes.push_back( msgBox );

        if(msgBox->isModal())
        {
            gInput.flushAll();
        }
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventSendSelectionDialogMsg>(evPtr) )
    {
        gMusicPlayer.stop();
        m_LevelPlay.stopMusic();

        std::unique_ptr<CMessageBoxSelection> pMsgBox( new CMessageBoxSelection( ev->Message, ev->Options ) );
        pMsgBox->init();

        mMessageBoxes.push_back( move(pMsgBox) );
    }
    else if( std::dynamic_pointer_cast<const EventEndGamePlay>(evPtr) )
    {
        m_endgame = true;
        // The last menu has been removed. Restore back the game status
        gBehaviorEngine.setPause(false);
        gMenuController.clearMenuStack();
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventEnterLevel>(evPtr) )
    {
        if(ev->data >= 0xC000)	// Start a new level!
        {
            // Ensure no one is dead anymore
            const int numPlayers = gBehaviorEngine.numPlayers();
            mDead.assign(numPlayers, false);
            mGameOver.assign(numPlayers, false);

            const Uint16 newLevel = ev->data - 0xC000;
            if(newLevel < 50)
            {
                if(m_LevelPlay.loadLevel(ev->mSprVar, newLevel))
                {
                    gAudio.playSound( int(GameSound::ENTER_LEVEL) );
                    mWorldMapActive = false;
                    m_LevelPlay.refreshGameplayMap();
                }
                else
                {
                    mWorldMapActive = true;
                    m_WorldMap.refreshGameplayMap();
                    showModalMsgWithBmp(0, "This Level seems to be broken",
                                   105, LEFT, false, nullptr);
                }
            }
        }

        gInput.flushFingerEvents();
    }
    else if( std::dynamic_pointer_cast<const EventRestartLevel>(evPtr) )
    {
        gMusicPlayer.stop();
        m_LevelPlay.reloadLevel();
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventExitLevel>(evPtr) )
    {
        // Ensure no one is dead anymore
        const int numPlayers = gBehaviorEngine.numPlayers();
        mDead.assign(numPlayers, false);
        mGameOver.assign(numPlayers, false);


        if( ev->playSound )
        {
            gAudio.playSound( int(GameSound::LEVEL_DONE) );
        }

        int newLevel = 0;

        GsLua lua;
        const auto ok = lua.loadFile(
                            JoinPaths(gKeenFiles.gameDir, "exitToLevel.lua"));

        if(ok)
        {
            const int oldLevel = m_LevelPlay.getLevelIdx();
            lua.runFunctionRetOneInt("exitTo", oldLevel, newLevel);
        }

        std::string levelLoadText = "LEVEL";
        levelLoadText += itoa(newLevel);
        levelLoadText += "_LOAD_TEXT";

        const std::string loading_text = gBehaviorEngine.getString(levelLoadText);

        mWorldMapActive = true;
        m_WorldMap.refreshGameplayMap();

        if(newLevel == 0)
        {
            m_WorldMap.loadAndPlayMusic();
        }

        const auto sprVar = mInventoryVec[ev->who].mSpriteVar;

        showModalMsgWithBmp( sprVar, loading_text, "KEENTHUMBSUPLOADING",
                        LEFT, true, nullptr);

        const EventExitLevel &evCopy = *ev;

        gEventManager.add( new EventPlayerEndLevel(evCopy) );

        if(newLevel != 0)
        {
            gMusicPlayer.stop();            
            m_LevelPlay.loadLevel(ev->who, newLevel);
            mWorldMapActive = false;
            m_LevelPlay.refreshGameplayMap();
        }
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventDieKeenPlayer>(evPtr) )
    {
        looseManagement(ev->playerID,
                        ev->levelObj,
                        ev->levelName);

    }
    else if( const auto ev = std::dynamic_pointer_cast<const RevivePlayer>(evPtr) )
    {        
        if(!mWorldMapActive)
        {
            // Check for an alive player and get his coordinates
            if(auto playerPtr =
                dynamic_cast<CPlayerBase*>( m_LevelPlay.getNextStandingStillPlayer() ))
            {
                if(!mGameOver[ev->mPlayerIdx])
                {
                    // Revive Player at those coordinates
                    const auto pos = playerPtr->getPosition();
                    m_LevelPlay.revivePlayerAt(ev->mPlayerIdx, pos);

                    // Remove dead vector entry
                    mDead[ev->mPlayerIdx] = false;
                }
                else
                {
                    gAudio.playSound(int(GameSound::CANT_DO));
                    galaxy::showMsg(0, "Game Over!", nullptr, false, "",
                                    galaxy::CMessageBoxGalaxy::Alignment::UPPERRIGHT,
                                    1000, false);

                }
            }
            else
            {
                gAudio.playSound(int(GameSound::CANT_DO));
                galaxy::showMsg(0, "For revive one must stand still", nullptr, false, "",
                                galaxy::CMessageBoxGalaxy::Alignment::UPPERRIGHT,
                                2000, false);
            }
        }
    }
    else if( const auto ev = std::dynamic_pointer_cast<const EventExitLevelWithFoot>(evPtr) )
    {
        gMusicPlayer.stop();
        mWorldMapActive = true;
        m_WorldMap.refreshGameplayMap();
        m_WorldMap.loadAndPlayMusic();
        gEventManager.add( new EventPlayerRideFoot(*ev) );
    }
    else if( const auto ev =  std::dynamic_pointer_cast<const EventPlayTrack>(evPtr) )
    {
        if(!mWorldMapActive)
        {
            m_LevelPlay.playMusic(ev->track);
        }
        else
        {
            m_WorldMap.playMusic(ev->track);
        }
    }
    else if(mWorldMapActive)
    {
        m_WorldMap.pumpEvent(evPtr);
    }
    else if(!mWorldMapActive)
    {
        m_LevelPlay.pumpEvent(evPtr);
    }
}



/**
 *  The main ingame process cycle when keen galaxy is up and running
 */
void CPlayGameGalaxy::ponder(const float deltaT)
{
    CPlayGame::ponder(deltaT);

    if( gMenuController.active() )
    {
        return;
    }

    processInput();

    // Check if Sandwich-Menu was clicked
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


    bool blockGamePlay = false;

    // if there is one modal message box, block the gameplay
    for(auto &msgBox : mMessageBoxes)
    {
        blockGamePlay |= msgBox->isModal();
    }

    int playerCount = 0;
    bool noLifesLeft = true; // Set gameover and check if player have enough lifes
                          // to stay in the game
    for( auto &inv : mInventoryVec )
    {
        // Trigger the Status screen here
        if(inv.showStatus())
        {
            if( gInput.getPressedAnyButtonCommand(playerCount) )
            {
                gAudio.playSound(int(GameSound::STATUS_SLIDE_OUT));
                inv.toggleStatusScreen();
            }
        }
        else
        {
            if(!blockGamePlay && gInput.getPressedCommand(playerCount, IC_STATUS))
            {
                CMapPlayGalaxy *pMap = &m_WorldMap;

                if(!mWorldMapActive)
                    pMap = &m_LevelPlay;

                inv.Item.mLevelName = pMap->getLevelName();

                gAudio.playSound(int(GameSound::STATUS_SLIDE_IN));

                inv.toggleStatusScreen();
            }
        }
        playerCount++;

        noLifesLeft &= (inv.Item.m_lifes<0);
    }


    if(noLifesLeft && !mGameOverTriggered) // Game over?
    {
        mGameOverTriggered = true;
        const std::string end_text("GAME OVER!\n");
        showMsg(0, end_text, new EventEndGamePlay());
        return;
    }

    // process inlevel play if active. At the start it's disabled, so m_WorldMap turns is active.
    if(mWorldMapActive)
    {
        m_WorldMap.setMsgBoxOpen(blockGamePlay);
        m_WorldMap.ponder(deltaT);
    }
    else
    {
        m_LevelPlay.setMsgBoxOpen(blockGamePlay);
        m_LevelPlay.ponder(deltaT);
    }

    // Do some logic of the message boxes
    for(auto &msgBox : mMessageBoxes)
    {
        msgBox->ponder(deltaT);
    }

    // Remove finished msg boxes from the list
    mMessageBoxes.remove_if( [](std::shared_ptr<CMessageBoxGalaxy> &msgBox)
                                {
                                    return msgBox->isFinished();
                                }
                            );


    if(blockGamePlay) return;

    // Cheat Codes
    auto &cheat = gBehaviorEngine.mCheatmode;
    if( gInput.getHoldedKey(KF10) )
    {
        if(gInput.getHoldedKey(KJ))
        {
            cheat.jump = !cheat.jump;
            std::string jumpstring = "Jump-Cheat has been ";
            jumpstring += ((cheat.jump) ? "enabled" : "disabled");
            showMsg(0, jumpstring);
        }
        else if(gInput.getHoldedKey(KG))
        {
            cheat.god = !cheat.god;
            std::string godstring = "God-Mode has been ";
            godstring += ((cheat.god) ? "enabled" : "disabled");
            showMsg(0, godstring);
        }
        else if(gInput.getHoldedKey(KI))
        {
            cheat.items = true;
        }
        else if(gInput.getHoldedKey(KN))
        {
            auto &noclip = gBehaviorEngine.mCheatmode.noclipping;
            noclip = !noclip;

            cheat.noclipping = noclip;

            std::string noclipTxt("No clipping ");
            noclipTxt += noclip ? "ON" : "OFF";
            showMsg(0, noclipTxt);
        }
        else if(gInput.getHoldedKey(KS))
        {
            for( auto &inv : mInventoryVec )
                inv.Item.triggerAllItemsCheat();

            cheat.items = true;
            cheat.god = true;
            cheat.jump = true;
            showMsg(0, "Super Cheat!");
        }
    }


    if(cheat.items)
    {
        showMsg(0, "Free items!");

        for( auto &inv : mInventoryVec )
            inv.Item.triggerAllItemsCheat();

       cheat.items = false;
    }

}

void CPlayGameGalaxy::render()
{
    if( gMenuController.active() )
    {
        return;
    }

    // Render World Map
    if(mWorldMapActive)
    {
        m_WorldMap.render();
    }
    else // Render the Level Scene
    {
        m_LevelPlay.render();
    }

    // Show Status screen
    for( auto &inv : mInventoryVec )
    {
        if( inv.showStatus() )
            inv.drawStatus();
    }

    for(auto &msgBox : mMessageBoxes)
    {
        msgBox->render();
    }



    const bool msgboxactive = !mMessageBoxes.empty();

    // The position of the sandwich menu depends on the configured blit resolution
    mMenuButtonRect.x = gVideoDriver.getBlitSurface()->w-mMenuButtonRect.w;

    if(gBehaviorEngine.mOptions[GameOption::SANDWICHMENU].value)
    {
        auto menuButtonRectWithBorder = mMenuButtonRect;
        menuButtonRectWithBorder.y += gVideoDriver.getVidConfig().mHorizBorders;

        // Draw a menu button where the mouse/finger might tap on
        drawMenuInGameButton(menuButtonRectWithBorder);
    }

    // Draw some Textboxes with Messages only if one of those is open and needs to be drawn
    if(msgboxactive)
    {
        mMessageBoxes.front()->render();
        return;
    }
}

void CPlayGameGalaxy::processInput()
{

#ifdef USE_VIRTUALPAD
#if SDL_VERSION_ATLEAST(2, 0, 0)
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
#endif
}


}
