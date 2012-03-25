/*
 * CMenu.cpp
 *
 *  Created on: 06.04.2010
 *      Author: gerstrong
 */

#include "CMenuGalaxy.h"

#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"

namespace galaxy
{

///
// Initialization
///
/**
 * \brief sets the menu up when it's opened (initialization)
 */
/*CMenuGalaxy::CMenuGalaxy( char menu_mode, CSaveGameController &SavedGame,
		  bool &restartVideo ) :
CMenu( menu_mode, SavedGame, DLG_THEME_GALAXY, restartVideo),
processPtr(&CMenuGalaxy::processMainMenu),
m_TitleBmp(*g_pGfxEngine->getBitmap("MAINMENULABEL"))
{
	// Perform coordinate calculations
	m_title_coord.x = (g_pVideoDriver->getGameResolution().w - m_TitleBmp.getWidth())/2-48;
	m_title_coord.y = 48;
	init(MAIN);
}

bool CMenuGalaxy::init( menutypes menu_type )
{
	cleanup();

	//CFont &Font = g_pGfxEngine->getFont(1);
	//Font.setBGColour(g_pVideoDriver->mp_VideoEngine->getBlitSurface()->format, 0x545454);

	CMenu::init(menu_type);

	switch(m_menu_type)
	{
	case MAIN:
		initMainMenu(); processPtr = &CMenuGalaxy::processMainMenu; break;
	default:
		processPtr = NULL;
		break;
	}
	return true;
}*/

///
// Processes
///
/**
 * \brief This cycle is processed every LPS. It's the menu core process
 */
void CMenuGalaxy::process()
{
	// Process the Menu Type logic.
	//CMenu::process();

	if(processPtr)
		(this->*processPtr)();
}

/**
 * \brief This draws the upper part of the galaxy menu border
 */
void CMenuGalaxy::drawMenuBorder()
{
	SDL_Rect rect;
	rect.x = m_title_coord.x-2;	rect.w = 155;
	rect.y = m_title_coord.y+7;	rect.h = 1;
	SDL_Surface *blit = g_pVideoDriver->mp_VideoEngine->getBlitSurface();

	// This one draws a line behind the bitmap
	SDL_FillRect(blit, &rect, 0x54fc54);

	// And this the bitmap itself
	m_TitleBmp.draw(blit, m_title_coord.x, m_title_coord.y);

	// This one draws a line at the down part
	rect.y = m_title_coord.y+90;
	SDL_FillRect(blit, &rect, 0x54fc54);

	// TODO: Missing the guide text here!
}

/**
 * \brief This is the child process only for the main menu itself
 */
void CMenuGalaxy::processMainMenu()
{
	drawMenuBorder();

	//if( m_selection != -1)
	{
		/*if( m_menu_mode == PASSIVE )
		{
			if( m_selection == 5 ) // Back to Demo
				m_demoback = true;

			else if( m_selection == 6 ) // Choose Game
				m_choosegame = true;
		}*/
		/*else if( m_menu_mode == ACTIVE )
		{
			if( m_selection == 5 ) // Back to Game
				m_mustclose = true;
		}*/
	}

	//if( m_selection == 3 ) // Show Highscores
	{
		// TODO: Code here will a special case and must be done!
	}
}

CMenuGalaxy::~CMenuGalaxy()
{

}

}
