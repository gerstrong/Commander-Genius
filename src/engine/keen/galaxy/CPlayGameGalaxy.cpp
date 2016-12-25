/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include <base/CInput.h>
#include <base/utils/StringUtils.h>
#include <base/video/CVideoDriver.h>
#include <widgets/GsMenuController.h>
#include <iostream>
#include <boost/property_tree/ptree.hpp>

#include "CPlayGameGalaxy.h"
#include "common/CMapLoaderGalaxy.h"
#include "graphics/GsGraphics.h"
#include "common/dialog/CMessageBoxBitmapGalaxy.h"
#include "common/dialog/CMessageBoxSelection.h"
#include "sdl/audio/Audio.h"
#include "sdl/audio/music/CMusic.h"
#include "graphics/effects/CColorMerge.h"
#include "graphics/effects/CDimDark.h"

#include "ep4/ai/CInchWorm.h"
#include "common/ai/CPlayerLevel.h"
#include "common/ai/CPlayerWM.h"
#include "engine/core/VGamepads/vgamepadsimple.h"
#include "menu/MainMenu.h"

#include <fileio/KeenFiles.h>
#include <base/GsArguments.h>


namespace galaxy
{



CPlayGameGalaxy::CPlayGameGalaxy(const int startlevel) :
CPlayGame(startlevel),
m_WorldMap( mInventoryVec),
m_LevelPlay( mInventoryVec),
m_SavedGame(*gpSaveGameController)
{
    const int numPlayers = gpBehaviorEngine->mPlayers;
    mDead.assign(numPlayers, false);
    mGameOver.assign(numPlayers, false);

    mInventoryVec.resize(numPlayers);

    for(int i=0 ; i<numPlayers ; i++)
    {
        mInventoryVec[i].setup(i);
    }

    m_WorldMap.init();

    m_Level = startlevel;

    // If no level has been set or is out of bound, set it to map.
    if(m_Level > 100 || m_Level < 0 )
        m_Level = WORLD_MAP_LEVEL_GALAXY;

    // Create the special merge effect (Fadeout)
    CColorMerge *pColorMergeFX = new CColorMerge(8);
    gEffectController.setupEffect(pColorMergeFX);
}

// NOTE: Only for compatibility mode. Since CG 1.5 it is only used for
// supporting older versions of Savegame states of CG
bool CPlayGameGalaxy::loadGameState()
{
	CSaveGameController &savedGame = *(gpSaveGameController);

	// This fills the datablock from CSavedGame object
	if(!savedGame.load())
		return false;

	// Create the special merge effect (Fadeout)
	CColorMerge *pColorMergeFX = new CColorMerge(8);

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	savedGame.decodeData(m_Episode);
    savedGame.decodeData(gpBehaviorEngine->mDifficulty);

	// Load number of Players
    savedGame.decodeData(gpBehaviorEngine->mPlayers);

	// We need to load both Levels first, before we do the writing from the saved state.

    mInventoryVec[0] << savedGame;

	bool active;
	savedGame.decodeData( active );
	m_WorldMap.setActive(active);
	m_WorldMap << savedGame;

	savedGame.decodeData( active );
	m_LevelPlay.setActive(active);
	if(active)
        m_LevelPlay<<savedGame;

	// Create the special merge effect (Fadeout)
    gEffectController.setupEffect(pColorMergeFX);

	return true;
}



bool CPlayGameGalaxy::loadXMLGameState()
{            
    /// Create tree for reading
    using boost::property_tree::ptree;
    ptree pt;

    CSaveGameController &savedGame = *(gpSaveGameController);
    if(!savedGame.loadXMLTree(pt))
        return false;

    /// Read the nodes and store the data as needed
    ptree &stateNode = pt.get_child("GameState");


    /// Load the Game in the CSavedGame object
    // Get the episode, and difficulty
    m_Episode = stateNode.get<int>("episode");
    gpBehaviorEngine->mDifficulty = static_cast<Difficulty>(stateNode.get<int>("difficulty", 1));

    // Get number of Players
    const unsigned int numPlayers = stateNode.get<int>("NumPlayer");
    gpBehaviorEngine->mPlayers = numPlayers;

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
            auto &invNode = playerNode.get_child("inventory");
            mInventoryVec[variant].setup(variant);
            mInventoryVec[variant] << invNode;
        }
    }


    bool active;
    ptree &wmNode = stateNode.get_child("WorldMap");
    active = wmNode.get<bool>("<xmlattr>.active", false);
    m_WorldMap.setActive(active);
    m_WorldMap << wmNode;

    ptree &levelPlayNode = stateNode.get_child("LevelPlay");
    active = levelPlayNode.get<bool>("<xmlattr>.active");
    m_LevelPlay.setActive(active);
    if( active )
    {
        m_LevelPlay << levelPlayNode;
    }

    return true;
}


bool CPlayGameGalaxy::saveXMLGameState()
{
    /// Create tree
    using boost::property_tree::ptree;
    ptree pt;

    /// Create the nodes and store the data as needed
    ptree &stateNode = pt.add("GameState", "");

    /// Save the Game in the CSavedGame object
    // store the episode, level and difficulty    
    stateNode.put("episode", int(m_Episode));


    /// Save the Game in the CSavedGame object
    // store the episode, level and difficulty
    stateNode.put("difficulty", gpBehaviorEngine->mDifficulty);

    // Save number of Players
    const unsigned int numPlayers = gpBehaviorEngine->mPlayers;
    stateNode.put("NumPlayer", numPlayers);

    ptree &deadNode = pt.add("death", "");
    for(unsigned int id = 0 ; id < mDead.size() ; id++ )
    {
        deadNode.put("<xmlattr>.player", id);
        deadNode.put("<xmlattr>.dead", mDead[id]);
        deadNode.put("<xmlattr>.gameover", mGameOver[id]);
    }

    for( unsigned int id=0 ; id<numPlayers ; id++ )
    {
        ptree &playerNode = pt.add("Player", "");
        playerNode.put("<xmlattr>.variant", id);
        auto &invNode = playerNode.put("inventory", "");
        mInventoryVec[id] >> invNode;
    }

    bool active = m_WorldMap.isActive();
    ptree &wmNode = stateNode.add("WorldMap", "");
    wmNode.put("<xmlattr>.active", active);
    m_WorldMap >> wmNode;

    active = m_LevelPlay.isActive();
    ptree &levelPlayNode = stateNode.add("LevelPlay", "");
    levelPlayNode.put("<xmlattr>.active", active);

    if( active )
        m_LevelPlay >> levelPlayNode;

    CSaveGameController &savedGame = *(gpSaveGameController);
    if(savedGame.saveXMLTree(pt))
        return true;

    return false;
}




// Setup for the ingame
bool CPlayGameGalaxy::init()
{
	if(m_Level == 0)
	{
		m_WorldMap.setActive(true);
		m_WorldMap.loadAndPlayMusic();
	}
	else
	{
		// manually a level has been loaded
		m_LevelPlay.loadLevel(m_Level);
		m_LevelPlay.setActive(true);
	}

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
};



struct EventRestartLevel : CEvent {};


void CPlayGameGalaxy::looseManagement( const int playerID,
                                       const bool playerGameOver,
                                       const int levelObj,
                                       const std::string &levelName )
{
    CEventContainer &eventContainer = gEventManager;

    // Check if all players are dead
    bool allDead = true;
    bool allGameOver = true;

    unsigned int nextAliveID = 0;

    while(mDead[nextAliveID]) nextAliveID++;

    mDead[playerID] = true;

    if(playerGameOver)
        mGameOver[playerID] = true;

    for(auto deadIt = mDead.begin() ; deadIt != mDead.end() ; deadIt++ )
    {
        allDead &= (*deadIt);
    }

    for(auto goIt = mGameOver.begin() ; goIt != mGameOver.end() ; goIt++ )
    {
        allGameOver &= (*goIt);
    }

    if(allGameOver) // Game over?
    {
        const std::string end_text("GAME OVER!\n");
        showMsg(end_text, new EventEndGamePlay());
        return;
    }
    else if(allDead) // not yet!
    {
        // Create the Event Selection screen
        std::string loosemsg  = "You didn't make it past\n";
        loosemsg += levelName;
        EventSendSelectionDialogMsg *pdialogevent = new EventSendSelectionDialogMsg(loosemsg);
        pdialogevent->addOption("Try Again", new EventRestartLevel() );

        std::string exitMsg = "Exit to " + gpBehaviorEngine->mapLevelName;
        pdialogevent->addOption(exitMsg, new EventExitLevel( levelObj, false, false, playerID) );
        eventContainer.add( pdialogevent );

        for(auto deadIt = mDead.begin() ; deadIt != mDead.end() ; deadIt++ )
            (*deadIt) = false;
    }

    // Now let's check for important items
    // If one player goes game over on of the other should get the item then.
    const unsigned int numDeadPlayers = mDead.size();
    if( numDeadPlayers > 1 && nextAliveID < numDeadPlayers )
    {
        CInventory &next = mInventoryVec[nextAliveID];
        CInventory &dying = mInventoryVec[playerID];

        next.fetchImportantStuff(dying);
    }

}


void CPlayGameGalaxy::pumpEvent(const CEvent *evPtr)
{
    // In this part we will poll all the relevant Events that are important for the
    // Galaxy Main Engine itself. For example, load map, setup world map, show Highscore
    // are some of those events.

    if( dynamic_cast<const SaveGameEvent*>(evPtr) )
    {
        saveXMLGameState();
        gInput.flushAll();
    }
    else if( const EventSendDialog *ev = dynamic_cast<const EventSendDialog*>(evPtr) )
    {
        mMessageBoxes.push_back( ev->mMsgBox );
        gInput.flushAll();
    }
    else if( const EventSendSelectionDialogMsg* ev = dynamic_cast<const EventSendSelectionDialogMsg*>(evPtr) )
    {
        g_pMusicPlayer->stop();
        std::unique_ptr<CMessageBoxSelection> pMsgBox( new CMessageBoxSelection( ev->Message, ev->Options ) );
        pMsgBox->init();

        mMessageBoxes.push_back( move(pMsgBox) );
    }
    else if( dynamic_cast<const EventEndGamePlay*>(evPtr) )
    {
        m_endgame = true;
        // The last menu has been removed. Restore back the game status
        gpBehaviorEngine->setPause(false);
        gMenuController.clearMenuStack();
    }
    else if( const EventEnterLevel *ev = dynamic_cast<const EventEnterLevel*>(evPtr) )
    {
        if(ev->data >= 0xC000)	// Start a new level!
        {
            const Uint16 NewLevel = ev->data - 0xC000;
            if(NewLevel < 50)
            {
                g_pMusicPlayer->stop();                
                m_WorldMap.setActive(false);
                m_LevelPlay.loadLevel(NewLevel);                
                g_pSound->playSound( SOUND_ENTER_LEVEL );
                m_LevelPlay.setActive(true);
            }
        }
    }
    else if( dynamic_cast<const EventRestartLevel*>(evPtr) )
    {
        g_pMusicPlayer->stop();
        m_LevelPlay.reloadLevel();
    }
    else if( const EventExitLevel *ev = dynamic_cast<const EventExitLevel*>(evPtr) )
    {                
        if( ev->playSound )
        {
            g_pSound->playSound( SOUND_LEVEL_DONE );
        }

        m_LevelPlay.setActive(false);
        m_WorldMap.setActive(true);
        m_WorldMap.loadAndPlayMusic();

        const std::string loading_text = gpBehaviorEngine->getString("WORLDMAP_LOAD_TEXT");
        showMsgWithBmp( loading_text, "KEENTHUMBSUP", LEFT);

        const EventExitLevel &evCopy = *ev;

        gEventManager.add( new EventPlayerEndLevel(evCopy) );
    }
    else if( const EventDieKeenPlayer *ev = dynamic_cast<const EventDieKeenPlayer*>(evPtr) )
    {
        looseManagement(ev->playerID,
                        ev->gameOver,
                        ev->levelObj,
                        ev->levelName);

    }
    else if( const EventExitLevelWithFoot *ev = dynamic_cast<const EventExitLevelWithFoot*>(evPtr) )
    {        
        g_pMusicPlayer->stop();
        m_LevelPlay.setActive(false);
        m_WorldMap.setActive(true);
        m_WorldMap.loadAndPlayMusic();
        gEventManager.add( new EventPlayerRideFoot(*ev) );
    }
    else if( const EventPlayTrack *ev =  dynamic_cast<const EventPlayTrack*>(evPtr) )
    {
        g_pMusicPlayer->stop();
        if( g_pMusicPlayer->loadTrack(ev->track) )
            g_pMusicPlayer->play();
    }
    else if(m_WorldMap.isActive())
    {
        m_WorldMap.pumpEvent(evPtr);
    }
    else if(m_LevelPlay.isActive())
    {
        m_LevelPlay.pumpEvent(evPtr);
    }
}

// Menu-Button
const SDL_Rect menuButtonRect = {310, 0, 10, 10};


/**
 *  The main ingame process cycle when keen galaxy is up and running
 */
void CPlayGameGalaxy::ponder(const float deltaT)
{
    if( g_pSound->pauseGamePlay() )
        return;

    if( gMenuController.active() )
        return;

    processInput();

    // Check if Sandwhich-Menu was clicked
    GsRect<float> rRect(menuButtonRect);
    const float w = gVideoDriver.getBlitSurface()->w;
    const float h = gVideoDriver.getBlitSurface()->h;
    rRect.x /= w;       rRect.y /= h;
    rRect.w /= w;       rRect.h /= h;

    if( checkSandwichMenuClicked(rRect) )
    {
        if( gMenuController.empty() ) // If no menu is open, open the main menu
        {
            gEventManager.add(new OpenMainMenuEvent());
        }
    }

    const bool msgboxactive = !mMessageBoxes.empty();

    int playerCount = 0;
    for( auto &inv : mInventoryVec )
    {
        // Trigger the Status screen here
        if(inv.showStatus())
        {
            if( gInput.getPressedAnyButtonCommand(playerCount) )
            {
                g_pSound->playSound(SOUND_STATUS_SLIDE_OUT);
                inv.toggleStatusScreen();
            }
        }
        else
        {
            if(!msgboxactive && gInput.getPressedCommand(playerCount, IC_STATUS))
            {
                g_pSound->playSound(SOUND_STATUS_SLIDE_IN);
                inv.toggleStatusScreen();
            }
        }

        playerCount++;
    }


    // process World Map if active. At the start it's enabled
    if(m_WorldMap.isActive())
    {
        m_WorldMap.setMsgBoxOpen(msgboxactive);
        m_WorldMap.ponder(deltaT);
    }

    // process inlevel play if active. At the start it's disabled, m_WorldMap turns it on.
    if(m_LevelPlay.isActive())
    {
        m_LevelPlay.setMsgBoxOpen(msgboxactive);
        m_LevelPlay.ponder(deltaT);
    }

    // Draw some Textboxes with Messages only if one of those is open and needs to be drawn
    if(msgboxactive)
    {
        CMessageBoxGalaxy *pMB = mMessageBoxes.front().get();
        pMB->ponder();

        if(pMB->isFinished() && !mMessageBoxes.empty())
        {
            mMessageBoxes.pop_front();
        }

        return;
    }

    //// Special Keyboard Input
    /// Cheat Codes
    auto &cheat = gpBehaviorEngine->mCheatmode;
    if( gInput.getHoldedKey(KF10) )
    {
        if(gInput.getHoldedKey(KJ))
        {
            cheat.jump = !cheat.jump;
            std::string jumpstring = "Jump-Cheat has been ";
            jumpstring += ((cheat.jump) ? "enabled" : "disabled");
            showMsg(jumpstring);
        }
        else if(gInput.getHoldedKey(KG))
        {
            cheat.god = !cheat.god;
            std::string godstring = "God-Mode has been ";
            godstring += ((cheat.god) ? "enabled" : "disabled");
            showMsg(godstring);
        }
        else if(gInput.getHoldedKey(KI))
        {
            cheat.items = true;
        }
        else if(gInput.getHoldedKey(KN))
        {
            cheat.noclipping = true;
            showMsg("No clipping toggle!");
        }
        else if(gInput.getHoldedKey(KS))
        {
            for( auto &inv : mInventoryVec )
                inv.Item.triggerAllItemsCheat();

            cheat.items = true;
            cheat.god = true;
            cheat.jump = true;
            showMsg("Super Cheat!");
        }
    }


    if(cheat.items)
    {
        showMsg("Free items!");

        for( auto &inv : mInventoryVec )
            inv.Item.triggerAllItemsCheat();

       cheat.items = false;
    }

    // menuButtonRect


}

void CPlayGameGalaxy::render()
{
    if( gMenuController.active() )
        return;

    // Render World Map
    if(m_WorldMap.isActive())
    {
        m_WorldMap.render();
    }

    // Render the Level Scene
    if(m_LevelPlay.isActive())
    {
        m_LevelPlay.render();
    }

    // Show Status screen
    for( auto &inv : mInventoryVec )
    {
        if( inv.showStatus() )
            inv.drawStatus();
    }

    const bool msgboxactive = !mMessageBoxes.empty();

    // Draw a menu button where the mouse/finger might tap on
    drawMenuInGameButton(menuButtonRect);

    // Draw some Textboxes with Messages only if one of those is open and needs to be drawn
    if(msgboxactive)
    {
        mMessageBoxes.front()->render();
        return;
    }
}

void CPlayGameGalaxy::processInput()
{

#ifdef TOUCHCONTROLS

    VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());

    if(!vkc)
    {
        gInput.mpVirtPad.reset(new VirtualKeenControl);
        gInput.mpVirtPad->init();
        vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());        
    }        

    assert(vkc);
    vkc->mShowDPad = true;

#endif
}


}
