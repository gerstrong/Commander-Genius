/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"
#include "CMapLoaderGalaxy.h"

#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "sdl/music/CMusic.h"
#include "StringUtils.h"

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, Uint8& difficulty,
		  CSavedGame &SavedGame) :
CPlayGame(ExeFile, level, numplayers, difficulty ),
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
		return false;
	}
	else
	{
		// manually a level has been loaded
		m_LevelPlay.loadLevel(m_Level);
		m_LevelPlay.setActive(true);
		return true;
	}
}

/**
 *  The main ingame process cycle when keen galaxy is up and running
 */
void CPlayGameGalaxy::process()
{
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

		if(g_pInput->getPressedCommand(IC_STATUS))
		{
			m_Inventory.toggleStatusScreen();
		}

		// process Page if one is open. Could be one of the finale scenes
		//if(m_Page.isActive())
		//m_Page.process();

		// We have to show the status screen, do so...
		if( m_Inventory.showStatus() )
		{
			m_Inventory.drawStatus();
		}
		else
		{
			// process World Map if active. At the start it's enabled
			if(m_WorldMap.isActive())
				m_WorldMap.process();

			// process World Map if active. At the start it's enabled
			if(m_LevelPlay.isActive())
				m_LevelPlay.process();
		}

		processRendering();
	}

	// In this part we will poll all the relevant Events that are important for the
	// Galaxy Main Engine itself. For example, load map, setup world map, show Highscore
	// are some of those events.
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	if( EventEnterLevel* ev = EventContainer.occurredEvent<EventEnterLevel>() )
	{
		// Start a new level!
		if(ev->data > 0xC000)
		{
			g_pMusicPlayer->stop();
			m_WorldMap.setActive(false);
			m_LevelPlay.loadLevel(ev->data - 0xC000);
			m_LevelPlay.setActive(true);
		}
		EventContainer.pop_Event();
	}
	else if( EventContainer.occurredEvent<EventExitLevel>() )
	{
		g_pMusicPlayer->stop();
		m_LevelPlay.setActive(false);
		m_WorldMap.setActive(true);
		m_WorldMap.loadAndPlayMusic();
	}


}

void CPlayGameGalaxy::processInput()
{
	// open the menu
	if(g_pInput->getPressedCommand(IC_QUIT))
	{
		mp_Menu = new CMenuGalaxy(ACTIVE, m_ExeFile, m_SavedGame, m_restartVideo);
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
