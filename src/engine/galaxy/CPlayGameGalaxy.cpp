/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"
#include "CMapLoaderGalaxy.h"

#include "graphics/CGfxEngine.h"
#include "dialog/CMessageBoxBitmapGalaxy.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "sdl/sound/CSound.h"
#include "sdl/music/CMusic.h"
#include "StringUtils.h"

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, Uint8& difficulty,
		  CSavedGame &SavedGame) :
CPlayGame(ExeFile, level, numplayers, difficulty ),
m_Inventory(difficulty, m_LevelName),
m_WorldMap(ExeFile, m_Inventory),
m_LevelPlay(ExeFile, m_Inventory),
mp_Menu(NULL),
m_SavedGame(SavedGame)
{
	m_WorldMap.init();
}

bool CPlayGameGalaxy::loadGameState()
{	return false;	}

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
	if(g_pSound->pauseGamePlay())
		return;

	if(mp_Menu) // In case the menu is open
	{
		// draw the title bitmap here!
		m_BackgroundBitmap.draw(g_pVideoDriver->mp_VideoEngine->getBlitSurface(), 0, 0);

		if(mp_Menu->mustClose())
		{
			g_pMusicPlayer->play();
			delete mp_Menu;
			mp_Menu = NULL;
		}
		else if(mp_Menu->getExitEvent())
			m_exitgame = true;
		else if(mp_Menu->mustEndGame())
			m_endgame = true;
		else
			mp_Menu->process();
	}
	else
	{
		processInput();

		// Trigger the Status screen here
		if(m_Inventory.showStatus())
		{
			if(g_pInput->getPressedAnyCommand())
			{
				g_pSound->playSound(SOUND_STATUS_SLIDE_OUT);
				m_Inventory.toggleStatusScreen();
			}
		}
		else
		{
			if(g_pInput->getPressedCommand(IC_STATUS))
			{
				g_pSound->playSound(SOUND_STATUS_SLIDE_IN);
				m_Inventory.toggleStatusScreen();
			}
		}


		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			m_Inventory.toggleStatusScreen();
		}

		// process Page if one is open. Could be one of the finale scenes
		//if(m_Page.isActive())
		//m_Page.process();

		// process World Map if active. At the start it's enabled
		if(m_WorldMap.isActive())
		{
			m_WorldMap.process();
		}

		// process World Map if active. At the start it's enabled
		if(m_LevelPlay.isActive())
		{
			m_LevelPlay.process();
		}

		// We have to show the status screen, do so...
		if( m_Inventory.showStatus() )
		{
			m_Inventory.drawStatus();
		}

		processRendering();
	}

	// In this part we will poll all the relevant Events that are important for the
	// Galaxy Main Engine itself. For example, load map, setup world map, show Highscore
	// are some of those events.
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	std::list<CMessageBoxGalaxy*>& MessageBoxQueue = m_LevelPlay.getMessageBoxQueue();

	if( EventSendBitmapDialogMsg* ev = EventContainer.occurredEvent<EventSendBitmapDialogMsg>() )
	{
		MessageBoxQueue.push_back(new CMessageBoxBitmapGalaxy( ev->Msg, ev->BitmapID, ev->Direction ) );
		EventContainer.pop_Event();
	}

	if(MessageBoxQueue.empty())
	{
		if( EventEnterLevel* ev = EventContainer.occurredEvent<EventEnterLevel>() )
		{
			// Start a new level!
			if(ev->data > 0xC000)
			{
				const Uint16 NewLevel = ev->data - 0xC000;
				g_pMusicPlayer->stop();
				m_WorldMap.setActive(false);
				m_LevelPlay.loadLevel(NewLevel);
				m_LevelName = m_LevelPlay.getLevelName();
				g_pSound->playSound( SOUND_ENTER_LEVEL );
				m_LevelPlay.setActive(true);
			}
			EventContainer.pop_Event();
		}
		else if( EventContainer.occurredEvent<EventExitLevel>() )
		{
			g_pMusicPlayer->stop();
			m_LevelPlay.setActive(false);
			m_WorldMap.setActive(true);
			m_LevelName = m_WorldMap.getLevelName();
			m_WorldMap.loadAndPlayMusic();
		}
	}
}

void CPlayGameGalaxy::processInput()
{
	// open the menu
	if(g_pInput->getPressedCommand(IC_QUIT))
	{
		mp_Menu = new CMenuGalaxy(ACTIVE, m_SavedGame, m_restartVideo);
		m_BackgroundBitmap = *g_pGfxEngine->getBitmap("KEENSWATCH");
		g_pMusicPlayer->pause();
	}
}

/*
 * This processes the rendering over all the other stuff. THis is used for extra messages normally
 * blit scrollsurface happens with m_WorldMap or m_LevelPlay
 */
void CPlayGameGalaxy::processRendering()
{

}


void CPlayGameGalaxy::cleanup()
{}

CPlayGameGalaxy::~CPlayGameGalaxy()
{
	if(mp_Menu)
		delete mp_Menu;
}

}
