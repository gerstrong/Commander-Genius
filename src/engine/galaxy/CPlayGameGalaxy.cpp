/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"
#include "common/CMapLoaderGalaxy.h"

#include "graphics/CGfxEngine.h"
#include "common/dialog/CMessageBoxBitmapGalaxy.h"
#include "common/dialog/CMessageBoxSelection.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "sdl/sound/CSound.h"
#include "sdl/music/CMusic.h"
#include "StringUtils.h"
#include "common/Menu/CMenuController.h"
#include "graphics/effects/CColorMerge.h"
#include "graphics/effects/CDimDark.h"

#include <iostream>

#include <boost/property_tree/ptree.hpp>

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, CSaveGameController &SavedGame) :
CPlayGame(ExeFile, level, numplayers ),
m_WorldMap(ExeFile, mInventoryVec, m_Cheatmode),
m_LevelPlay(ExeFile, mInventoryVec, m_Cheatmode),
m_SavedGame(SavedGame)
{
    mDead.assign(numplayers, false);
    mGameOver.assign(numplayers, false);

    /*if(!mInventoryVec.empty())
        mInventoryVec.clear();

    for(int i=0 ; i<numplayers ; i++)
        mInventoryVec.push_back(CInventory(0));*/

    mInventoryVec.resize(numplayers);

    for(int i=0 ; i<numplayers ; i++)
    {
        mInventoryVec[i].setup(i, numplayers);
    }

    m_WorldMap.init();
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
	savedGame.decodeData(g_pBehaviorEngine->mDifficulty);

	// Load number of Players
	savedGame.decodeData(m_NumPlayers);

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
	g_pGfxEngine->setupEffect(pColorMergeFX);

	return true;
}


/*bool CPlayGameGalaxy::saveGameState()
{
	CSaveGameController &savedGame = *(gpSaveGameController);

	/// Save the Game in the CSavedGame object
	// store the episode, level and difficulty
	savedGame.encodeData(m_Episode);
	savedGame.encodeData(g_pBehaviorEngine->mDifficulty);

	// Save number of Players
	savedGame.encodeData(m_NumPlayers);

	m_Inventory >> savedGame;

	bool active = m_WorldMap.isActive();
	savedGame.encodeData( active );
	m_WorldMap >> savedGame;

	active = m_LevelPlay.isActive();
	savedGame.encodeData( active );

	if( active )
		m_LevelPlay >> savedGame;


	return savedGame.save();
}*/

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
    g_pBehaviorEngine->mDifficulty = static_cast<Difficulty>(stateNode.get<int>("difficulty", 1));

    // Get number of Players
    m_NumPlayers = stateNode.get<int>("NumPlayer");

    for( unsigned int id=0 ; id<m_NumPlayers ; id++ )
    {
        ptree &playerNode = pt.get_child("Player");
        //id = playerNode.get<int>("<xmlattr>.id", );
        auto &invNode = playerNode.get_child("inventory");
        mInventoryVec[id] << invNode;
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
        m_LevelPlay << levelPlayNode;

    return true;
}


/*void CPlayGameGalaxy::operator<<(boost::property_tree::ptree &invNode)
{
    m_Inventory<<invNode;
}*/


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
    stateNode.put("difficulty", g_pBehaviorEngine->mDifficulty);

    // Save number of Players
    stateNode.put("NumPlayer", m_NumPlayers);

    for( unsigned int id=0 ; id<m_NumPlayers ; id++ )
    {
        ptree &playerNode = pt.add("Player", "");
        playerNode.put("<xmlattr>.id", id);
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
 *  The main ingame process cycle when keen galaxy is up and running
 */
void CPlayGameGalaxy::ponder()
{
	if(g_pSound->pauseGamePlay() )
		return;

	CEventContainer &eventContainer = g_pBehaviorEngine->m_EventList;
	
	eventContainer.update();

	if( !gpMenuController->active() )
	{
	processInput();

	const bool msgboxactive = !mMessageBoxes.empty();

        int playerCount = 0;
        for( auto &inv : mInventoryVec )
        {
            // Trigger the Status screen here
            if(inv.showStatus())
            {
                if( g_pInput->getPressedAnyButtonCommand(playerCount) )
                {
                    g_pSound->playSound(SOUND_STATUS_SLIDE_OUT);
                    inv.toggleStatusScreen();
                }
            }
            else
            {
                if(!msgboxactive && g_pInput->getPressedCommand(playerCount, IC_STATUS))
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
            m_WorldMap.ponder();
		}

		// process World Map if active. At the start it's disabled, m_WorldMap turns it on.
		if(m_LevelPlay.isActive())
		{
            m_LevelPlay.setMsgBoxOpen(msgboxactive);
            m_LevelPlay.ponder();
		}                

		// Draw some Textboxes with Messages only if one of those is open and needs to be drawn
		if(msgboxactive)
		{
			CMessageBoxGalaxy *pMB = mMessageBoxes.front().get();
            pMB->ponder();

			if(pMB->isFinished())
				mMessageBoxes.pop_front();

			return;
		}

		//// Special Keyboard Input
		/// Cheat Codes
		if( g_pInput->getHoldedKey(KF10) )
		{
			if(g_pInput->getHoldedKey(KJ))
			{
				m_Cheatmode.jump = !m_Cheatmode.jump;
				std::string jumpstring = "Jump-Cheat has been ";
				jumpstring += ((m_Cheatmode.jump) ? "enabled" : "disabled");
				eventContainer.add( new EventSendDialog(jumpstring) );
			}
			else if(g_pInput->getHoldedKey(KG))
			{
				m_Cheatmode.god = !m_Cheatmode.god;
				std::string godstring = "God-Mode has been ";
				godstring += ((m_Cheatmode.god) ? "enabled" : "disabled");
				eventContainer.add( new EventSendDialog(godstring) );
			}
			else if(g_pInput->getHoldedKey(KI))
			{
				eventContainer.add( new EventSendDialog("Get all Items!") );

                for( auto &inv : mInventoryVec )
                    inv.Item.triggerAllItemsCheat();

				m_Cheatmode.items = true;
			}
			else if(g_pInput->getHoldedKey(KN))
			{
				m_Cheatmode.noclipping = true;
				eventContainer.add( new EventSendDialog("No clipping toggle!") );
			}
			else if(g_pInput->getHoldedKey(KS))
			{
		for( auto &inv : mInventoryVec )
		    inv.Item.triggerAllItemsCheat();

				m_Cheatmode.items = true;
				m_Cheatmode.god = true;
				m_Cheatmode.jump = true;
				eventContainer.add( new EventSendDialog("Super Cheat!") );
			}
		}

	}

	// In this part we will poll all the relevant Events that are important for the
	// Galaxy Main Engine itself. For example, load map, setup world map, show Highscore
	// are some of those events.

	if( EventSendBitmapDialogMsg *ev = eventContainer.occurredEvent<EventSendBitmapDialogMsg>() )
	{
		std::unique_ptr<CMessageBoxBitmapGalaxy> pMsgBox( new CMessageBoxBitmapGalaxy( ev->Msg, ev->BitmapRef, ev->Direction ) );
		pMsgBox->init();

		// Create the special merge effect (Fadeout) if requested
		if( g_pGfxEngine->runningEffect() )
		{
		    CColorMerge *pColorMerge = dynamic_cast<CColorMerge*>(g_pGfxEngine->Effect());
		    if( pColorMerge != NULL )
		    {
                SDL_Surface *fxSfc = pColorMerge->getSfc().get();
                SDL_Rect cutRect = pMsgBox->getRect();
                SDL_Surface *msgSfc = pMsgBox->getSfc();
                SDL_BlitSurface(msgSfc, NULL, fxSfc, &cutRect);
		    }

		    CDimDark *pDimDark = dynamic_cast<CDimDark*>(g_pGfxEngine->Effect());
		    if( pDimDark != NULL )
		    {
                SDL_Surface *fxSfc = pDimDark->getSfc().get();
                SDL_Surface *darkSfc = pDimDark->getDarkSfc().get();
                SDL_Rect cutRect = pMsgBox->getRect();
                SDL_Surface *msgSfc = pMsgBox->getSfc();
                SDL_BlitSurface(msgSfc, NULL, fxSfc, &cutRect);
                SDL_BlitSurface(msgSfc, NULL, darkSfc, &cutRect);
		    }
		}

		g_pInput->flushAll();
		mMessageBoxes.push_back( move(pMsgBox) );
		eventContainer.pop_Event();
	}
	else if( EventSendBitmapDialogMessages *ev = eventContainer.occurredEvent<EventSendBitmapDialogMessages>() )
	{
		for( auto &it : ev->msgs )
		{
			std::unique_ptr<CMessageBoxBitmapGalaxy> pMsgBox( new CMessageBoxBitmapGalaxy( it->Msg, it->BitmapRef, it->Direction ) );
			pMsgBox->init();

			mMessageBoxes.push_back( move(pMsgBox) );
		}
		g_pInput->flushAll();
		eventContainer.pop_Event();
	}
	else if( EventSendDialog *ev = eventContainer.occurredEvent<EventSendDialog>() )
	{
		std::unique_ptr<CMessageBoxGalaxy> pMsgBox( new CMessageBoxGalaxy( ev->Msg ) );
		pMsgBox->init();

		mMessageBoxes.push_back( move(pMsgBox) );
		g_pInput->flushAll();
		eventContainer.pop_Event();
	}
	else if( EventSendSelectionDialogMsg* ev = eventContainer.occurredEvent<EventSendSelectionDialogMsg>() )
	{
		g_pMusicPlayer->stop();
		std::unique_ptr<CMessageBoxSelection> pMsgBox( new CMessageBoxSelection( ev->Message, ev->Options ) );
		pMsgBox->init();

		mMessageBoxes.push_back( move(pMsgBox) );
		eventContainer.pop_Event();
	}


	if(mMessageBoxes.empty())
	{
		if( EventEnterLevel *ev = eventContainer.occurredEvent<EventEnterLevel>() )
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
			eventContainer.pop_Event();
		}
		else if( eventContainer.occurredEvent<EventRestartLevel>() )
		{
			g_pMusicPlayer->stop();
			m_LevelPlay.reloadLevel();
			eventContainer.pop_Event();
		}
		else if( EventExitLevel *ev = eventContainer.occurredEvent<EventExitLevel>() )
		{
			m_LevelPlay.setActive(false);
			m_WorldMap.setActive(true);
			m_WorldMap.loadAndPlayMusic();

			const EventExitLevel &evCopy = *ev;

			eventContainer.add( new EventPlayerEndLevel(evCopy) );

            // Should only happen in Keen 5. This should trigger on map teleportation
			if(evCopy.teleport)
			{
			  eventContainer.add( new EventPlayerTeleportFromLevel() );
			}

			eventContainer.pop_Event();
		}
        else if( EventDieKeenPlayer *ev = eventContainer.occurredEvent<EventDieKeenPlayer>() )
        {
            // Check if all players are dead
            bool allDead = true;
            bool allGameOver = true;

            const int playerID = ev->playerID;
            const bool playerGameOver = ev->gameOver;
            const int levelObj = ev->levelObj;
            const std::string levelName = ev->levelName;

            eventContainer.pop_Event();

            mDead[playerID] = true;

            if(playerGameOver)
                mGameOver[playerID] = true;

            for(auto deadIt = mDead.begin() ; deadIt != mDead.end() ; deadIt++ )
                allDead &= (*deadIt);

            for(auto goIt = mGameOver.begin() ; goIt != mGameOver.end() ; goIt++ )
                allGameOver &= (*goIt);

            if(allGameOver) // Game over?
            {
                const std::string end_text("GAME OVER!\n");
                eventContainer.add( new EventSendDialog(end_text) );
                eventContainer.add( new EventEndGamePlay() );
            }
            else if(allDead) // not yet!
            {
                // Create the Event Selection screen
                std::string loosemsg  = "You didn't make it past\n";
                loosemsg += levelName;
                EventSendSelectionDialogMsg *pdialogevent = new EventSendSelectionDialogMsg(loosemsg);
                pdialogevent->addOption("Try Again", new EventRestartLevel() );

                std::string exitMsg = "Exit to " + g_pBehaviorEngine->mapLevelName;
                pdialogevent->addOption(exitMsg, new EventExitLevel( levelObj, false, false, playerID) );
                eventContainer.add( pdialogevent );

                for(auto deadIt = mDead.begin() ; deadIt != mDead.end() ; deadIt++ )
                    (*deadIt) = false;
            }

        }
		else if( EventExitLevelWithFoot *ev = eventContainer.occurredEvent<EventExitLevelWithFoot>() )
		{
			g_pMusicPlayer->stop();
			m_LevelPlay.setActive(false);
			m_WorldMap.setActive(true);
			m_WorldMap.loadAndPlayMusic();
			eventContainer.add( new EventPlayerRideFoot(*ev) );
			eventContainer.pop_Event();
		}
		else if( EventPlayTrack *ev =  eventContainer.occurredEvent<EventPlayTrack>() )
		{
			g_pMusicPlayer->stop();
			if( g_pMusicPlayer->loadTrack(m_ExeFile, ev->track) )
				g_pMusicPlayer->play();
			eventContainer.pop_Event();
		}
		else if( eventContainer.occurredEvent<EventEndGamePlay>() )
		{
			m_endgame = true;
			eventContainer.pop_Event();
		}
	}
}

void CPlayGameGalaxy::render()
{
    if( !gpMenuController->active() )
    {

        // process World Map if active. At the start it's enabled
        if(m_WorldMap.isActive())
        {
            m_WorldMap.render();
        }

        // process World Map if active. At the start it's disabled, m_WorldMap turns it on.
        if(m_LevelPlay.isActive())
        {
            m_LevelPlay.render();
        }

        // We have to show the status screen, do so...
        for( auto &inv : mInventoryVec )
        {
            if( inv.showStatus() )
                inv.drawStatus();
        }

        const bool msgboxactive = !mMessageBoxes.empty();

        // Draw some Textboxes with Messages only if one of those is open and needs to be drawn
        if(msgboxactive)
        {
            mMessageBoxes.front()->render();
            return;
        }
    }      
}

void CPlayGameGalaxy::processInput()
{}


}
