/*
 * CPassive.cpp
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 *
 */

#include "CPassive.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CInput.h"

CPassive::CPassive(char Episode, std::string DataDirectory) {
	mp_IntroScreen = NULL;
	mp_menu = NULL;
	mp_map = NULL;

	m_mode = INTRO;
	m_Episode = Episode;
	m_DataDirectory = DataDirectory;
	mp_Tilemap = g_pGfxEngine->Tilemap;
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
}

CPassive::~CPassive() {
	// TODO Auto-generated destructor stub
}

bool CPassive::init(char mode)
{
	m_mode = mode;
	if( m_mode == INTRO )
	{
		mp_IntroScreen = new CIntro();
		mp_IntroScreen->init();
		mp_map = new CMap( mp_Scrollsurface, mp_Tilemap );
		mp_map->loadMap( m_Episode, 90, m_DataDirectory);
		mp_map->gotoPos( 64+5*320, 32);
		mp_map->drawAll();
	}
	else
		return false;

	return true;
}

void CPassive::process()
{
	// Check Inputs
	if( g_pInput->getPressedAnyKey() && mp_menu==NULL )
	{
		mp_menu = new CMenu( CMenu::PASSIVE );
		mp_menu->init();
	}
	else if( (g_pInput->getPressedKey(KQUIT) || g_pInput->getPressedCommand(IC_FIRE)) && mp_menu!=NULL )
	{
		delete mp_menu;
		mp_menu = NULL;
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
		// Title Screen code goes here!
		cleanup();
		init(DEMO);
	}
	else if( m_mode == DEMO )
	{
		// Demo modes are processed here!
		// TODO: Implement Demos here!
		cleanup();
		init(INTRO);
	}

	// Animate the tiles
	g_pGfxEngine->Tilemap->animateAllTiles( mp_Scrollsurface );

	// Blit the background
	g_pVideoDriver->blitScrollSurface(mp_map->m_scrollx_buf, mp_map->m_scrolly_buf);

	// If Menu is open show it!
	if( mp_menu != NULL )
	{
		mp_menu->process();

		if(mp_menu->getExitEvent())
		{
			delete mp_menu;
			mp_menu = NULL;
			cleanup();
			m_mode = SHUTDOWN;
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
		delete mp_map;
	}
	else if( m_mode == TITLE )
	{
		// Title Screen code goes here!
	}
	else if( m_mode == DEMO )
	{
		// Demo modes are processed here!
	}
}


