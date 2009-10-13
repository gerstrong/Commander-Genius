/*
 * CPassive.cpp
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 */

#include "CPassive.h"
#include "../include/fileio/story.h"
#include "../graphics/CGfxEngine.h"
#include "../common/CMenu.h"
#include "../common/CMapLoader.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"

CPassive::CPassive(char Episode, std::string DataDirectory) {
	mp_IntroScreen = NULL;
	mp_TitleScreen = NULL;
	mp_Menu = NULL;
	mp_Map = NULL;
	
	m_modeg = false;
	m_GoDemo = false;
	m_mode = INTRO;
	m_Episode = Episode;
	m_DataDirectory = DataDirectory;
	mp_Tilemap = g_pGfxEngine->Tilemap;
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	m_textsize = 0;
	m_NumPlayers = 0; // because no game chosen
}

bool CPassive::init(char mode)
{
	m_mode = mode;
	if( m_mode == INTRO )
	{
		mp_IntroScreen = new CIntro();
		mp_Map = new CMap( mp_Scrollsurface, mp_Tilemap);
		CMapLoader MapLoader( mp_Map );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		mp_Map->gotoPos( 64+5*320, 32); // Coordinates of star sky
		mp_Map->drawAll();
		mp_IntroScreen->init();
	}
	else if( m_mode == TITLE )
	{
		mp_TitleScreen = new CTitle();
		mp_Map = new CMap( mp_Scrollsurface, mp_Tilemap);
		CMapLoader MapLoader( mp_Map );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		mp_Map->gotoPos( 32, 32); // Coordinates of title screen 
		mp_Map->drawAll();
		mp_TitleScreen->init();				
	}
	else if( m_mode == DEMO )
	{
		// Setup the demo environment
	}
	else
		return false;

	return true;
}

////
// Process Routine
////
void CPassive::process()
{
	// Check Inputs

	// Open the Main-Menu or close the opened one?
	if( g_pInput->getPressedAnyKey() && mp_Menu==NULL )
	{
		g_pInput->flushAll();
		if (m_mode != TITLE)
		{
			cleanup();
			init(TITLE);
		}
		mp_Menu = new CMenu( CMenu::PASSIVE );
		mp_Menu->init();
	}
	else if( mp_Menu!=NULL )
	{
		if ( mp_Menu->m_demoback )
		{
			delete mp_Menu;
			mp_Menu = NULL;
		}
	}

	// Modes. We have three: Intro, Main-tile and Demos. We could add more.
	if( m_mode == INTRO )
	{
		// Intro code goes here!
		mp_IntroScreen->process();

		if( mp_IntroScreen->isFinished() )
		{
			// Shutdown mp_IntroScreen and show load Title Screen
			cleanup();
			init(TITLE);
		}
	}
	else if( m_mode == TITLE )
	{
		mp_TitleScreen->process();

		if( mp_TitleScreen->isFinished() )
		{
			// The Title screen was shown enough time, shut it down
			// and load Demo environment	
			cleanup();
			init(DEMO);
		}
	}
	else if( m_mode == DEMO )
	{
		// TODO: Demo modes are processed here!
		// TODO: Implement Demos here!
		cleanup();
		init(INTRO);
	}

	// Animate the tiles
	g_pGfxEngine->Tilemap->animateAllTiles( mp_Scrollsurface );

	// We may need to draw sprites here when in Demo mode. Check out
	// how to simplify that!
	// vectorize sprites. When they must be drawn the size is greater than 0

	// Blit the background
	g_pVideoDriver->blitScrollSurface(mp_Map->m_scrollx_buf, mp_Map->m_scrolly_buf);

	// If Menu is open show it!
	if( mp_Menu != NULL )
	{
		mp_Menu->process();

		if(mp_Menu->mustStartGame())
		{
			m_NumPlayers = mp_Menu->getNumPlayers();
			m_Difficulty = mp_Menu->getDifficulty();
			delete mp_Menu;
			mp_Menu = NULL;
			cleanup();
			m_mode = STARTGAME;
		}
		else if(mp_Menu->getExitEvent())
		{
			delete mp_Menu;
			mp_Menu = NULL;
			cleanup();
			m_mode = SHUTDOWN;
		}
		else if(mp_Menu->getShowStory())
		{
			delete mp_Menu;
			mp_Menu = NULL;
			m_textsize = readStoryText(&m_text, m_Episode, m_DataDirectory); // Read text from
			// and store it at the text pointer

			if(m_textsize > 0)
			{
				CMapLoader MapLoader( mp_Map );
				MapLoader.load( m_Episode, 90, m_DataDirectory);
				mp_Map->drawAll();
				mp_Menu->showPage(m_text,m_textsize);

				free(m_text);
			}
			cleanup();
		}
		else if(mp_Menu->getChooseGame())
		{
			delete mp_Menu;
			mp_Menu = NULL;
			cleanup();
			m_modeg = true;
		}
	}
}

////
// Cleanup. It is called at every mode change
////
void CPassive::cleanup()
{
	if( m_mode == INTRO )
	{
		delete mp_IntroScreen;
		delete mp_Map;
	}
	else if( m_mode == TITLE )
	{
		delete mp_TitleScreen;
		delete mp_Map;
	}
	else if( m_mode == DEMO )
	{
		// Demo modes are processed here!
		//delete mp_Map;
	}
}

CPassive::~CPassive() {
	// TODO Auto-generated destructor stub
}

