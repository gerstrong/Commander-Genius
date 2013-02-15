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

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, CSaveGameController &SavedGame) :
CPlayGame(ExeFile, level, numplayers ),
m_Inventory(m_LevelName),
m_WorldMap(ExeFile, m_Inventory, m_Cheatmode),
m_LevelPlay(ExeFile, m_Inventory, m_Cheatmode),
m_SavedGame(SavedGame)
{
    m_WorldMap.init();
}

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

	m_Inventory << savedGame;

	bool active;
	savedGame.decodeData( active );
	m_WorldMap.setActive(active);
	m_WorldMap << savedGame;

	savedGame.decodeData( active );
	m_LevelPlay.setActive(active);
	if(active)
		m_LevelPlay << savedGame;

	// Create the special merge effect (Fadeout)
	g_pGfxEngine->setupEffect(pColorMergeFX);

	return true;
}

bool CPlayGameGalaxy::saveGameState()
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
}


// Setup for the ingame
bool CPlayGameGalaxy::init()
{
	if(m_Level == 0)
	{
		m_WorldMap.setActive(true);
		m_WorldMap.loadAndPlayMusic();
		m_LevelName = m_WorldMap.getLevelName();
	}
	else
	{
		// manually a level has been loaded
		m_LevelPlay.loadLevel(m_Level);
		m_LevelPlay.setActive(true);
		m_LevelName = m_LevelPlay.getLevelName();
	}

	return true;
}

/**
 *  The main ingame process cycle when keen galaxy is up and running
 */
void CPlayGameGalaxy::process()
{
	if(g_pSound->pauseGamePlay() )
		return;

	CEventContainer &eventContainer = g_pBehaviorEngine->m_EventList;
	
	eventContainer.update();

	if( !gpMenuController->active() )
	{
		processInput();

		const bool msgboxactive = !mMessageBoxes.empty();

		// Trigger the Status screen here
		if(m_Inventory.showStatus())
		{
			if( g_pInput->getPressedAnyButtonCommand(0) )
			{
				g_pSound->playSound(SOUND_STATUS_SLIDE_OUT);
				m_Inventory.toggleStatusScreen();
			}
		}
		else
		{
			if(!msgboxactive && g_pInput->getPressedCommand(IC_STATUS))
			{
				g_pSound->playSound(SOUND_STATUS_SLIDE_IN);
				m_Inventory.toggleStatusScreen();
			}
		}

		// process World Map if active. At the start it's enabled
		if(m_WorldMap.isActive())
		{
			m_WorldMap.process(msgboxactive);
		}

		// process World Map if active. At the start it's disabled, m_WorldMap turns it on.
		if(m_LevelPlay.isActive())
		{
			m_LevelPlay.process(msgboxactive);
		}

		// We have to show the status screen, do so...
		if( m_Inventory.showStatus() )
		{
			m_Inventory.drawStatus();
		}

		// Draw some Textboxes with Messages only if one of those is open and needs to be drawn
		if(msgboxactive)
		{
			CMessageBoxGalaxy *pMB = mMessageBoxes.front().get();
			pMB->process();

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
				m_Inventory.Item.triggerAllItemsCheat();
				m_Cheatmode.items = true;
			}
			else if(g_pInput->getHoldedKey(KN))
			{
				m_Cheatmode.noclipping = true;
				eventContainer.add( new EventSendDialog("No clipping toggle!") );
			}
			else if(g_pInput->getHoldedKey(KS))
			{
				m_Inventory.Item.triggerAllItemsCheat();
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
					m_LevelName = m_LevelPlay.getLevelName();
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
			m_LevelName = m_WorldMap.getLevelName();
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
		else if( EventExitLevelWithFoot *ev = eventContainer.occurredEvent<EventExitLevelWithFoot>() )
		{
			g_pMusicPlayer->stop();
			m_LevelPlay.setActive(false);
			m_WorldMap.setActive(true);
			m_LevelName = m_WorldMap.getLevelName();
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

void CPlayGameGalaxy::processInput()
{}



}
