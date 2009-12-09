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

#define SAFE_DELETE(x) if(x!=NULL){ delete x; x = NULL;}

CPassive::CPassive(char Episode, std::string DataDirectory,
					CSavedGame &SavedGame, stOption *p_Option) :
	m_SavedGame(SavedGame),
	mp_Map(NULL),
	mp_Option(p_Option)
{
	mp_IntroScreen = NULL;
	mp_TitleScreen = NULL;
	mp_Menu = NULL;
	mp_PressAnyBox=NULL;
	
	m_modeg = false;
	m_GoDemo = false;
	m_mode = INTRO;
	m_Episode = Episode;
	m_DataDirectory = DataDirectory;
	m_textsize = 0;
	m_NumPlayers = 0; // because no game chosen
	m_hideobjects = false;
}

bool CPassive::init(char mode)
{
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	mp_Tilemap = g_pGfxEngine->Tilemap;
	m_mode = mode;
	if( m_mode == INTRO )
	{
		mp_IntroScreen = new CIntro();
		mp_Map = new CMap( mp_Scrollsurface, mp_Tilemap);
		CMapLoader MapLoader( mp_Map );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		mp_Map->gotoPos( 64+5*320, 16); // Coordinates of star sky
		mp_Map->drawAll();
		mp_IntroScreen->init();
	}
	else if( m_mode == TITLE )
	{
		mp_TitleScreen = new CTitle(m_object);
		mp_Map = new CMap( mp_Scrollsurface, mp_Tilemap);
		CMapLoader MapLoader( mp_Map );
		MapLoader.load( m_Episode, 90, m_DataDirectory);
		SDL_Rect gamerect = g_pVideoDriver->getGameResolution();
		mp_Map->gotoPos( 32, (gamerect.h<240) ? 32 : 0); // Coordinates of title screen
		mp_Map->drawAll();
		mp_TitleScreen->init(m_Episode);
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
	// Open the Main-Menu or close the opened one?
	if( mp_Menu==NULL )
	{
		if (mp_PressAnyBox==NULL && m_mode == TITLE)
		{
		mp_PressAnyBox = new CTextBox(150, 10," PRESS ANY KEY ");
		mp_PressAnyBox->setAttribs(0, LETTER_TYPE_RED);
		mp_PressAnyBox->enableBorders(true);
		}
		
		if (g_pInput->getPressedAnyKey())
		{
		// Close the "Press Any Key" box
		g_pInput->flushAll();
		if (m_mode != TITLE)
		{
			cleanup();
			init(TITLE);
		}
		else
		{
			SAFE_DELETE(mp_PressAnyBox);
			mp_Menu = new CMenu( PASSIVE, m_DataDirectory,
								m_Episode, *mp_Map, m_SavedGame, mp_Option );
			mp_Menu->init();
		}
		}
	}
	else if( mp_Menu!=NULL ) // Close menu
	{
		if ( mp_Menu->m_demoback )
		{
			SAFE_DELETE(mp_Menu);
			mp_Map->drawAll();
		}
		else if( mp_Menu->restartVideo())
		{
			cleanup();
			init(m_mode);
			mp_Menu->videoRestarted();
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
		
		if( mp_Menu == NULL )
		{
			if( mp_TitleScreen->isFinished() )
			{
				// The Title screen was shown enough time, shut it down
				// and load Demo environment
				cleanup();
				init(DEMO);
			}
		}
	}
	else if( m_mode == DEMO )
	{
		// TODO: Demo modes are processed here!
		// TODO: Implement Demos here!
		cleanup();
		init(TITLE);
	}

	// Animate the tiles
	mp_Map->animateAllTiles();

	// Blit the background
	g_pVideoDriver->blitScrollSurface();

	if(!m_hideobjects)
	{
		// Make the Objects do its jobs
		std::vector<CObject*>::iterator i;
		for( i=m_object.begin() ; i!=m_object.end() ; i++ )
		{
			(*i)->process();
		}
	}
	
	// If Menu is not open show "Press Any Key"
	if(mp_PressAnyBox != NULL)
		mp_PressAnyBox->process();
	
	// If Menu is open show it!
	if( mp_Menu != NULL )
	{
		mp_Menu->process();
		
		// Let the menu control, if objects are to be seen or hidden
		m_hideobjects = mp_Menu->m_hideobjects;

		if(mp_Menu->mustStartGame() || m_SavedGame.getCommand() == CSavedGame::LOAD) // Start a normal game
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
		else if(mp_Menu->getChooseGame())
		{
			delete mp_Menu;
			mp_Menu = NULL;
			m_modeg = true;
		}
		else if(mp_Menu->restartVideo()) // When some video settings has been changed
		{
			cleanup();
			init(m_mode);
			mp_Menu->videoRestarted();
		}
	}
}

////
// Cleanup. It is called at every mode change
////
void CPassive::cleanup()
{
	m_object.clear();

	if( m_mode == INTRO )
	{
		delete mp_IntroScreen;
		SAFE_DELETE(mp_Map);
	}
	else if( m_mode == TITLE )
	{
		mp_TitleScreen->cleanup();
		delete mp_TitleScreen;
		SAFE_DELETE(mp_Map);
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

