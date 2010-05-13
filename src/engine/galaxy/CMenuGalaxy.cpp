/*
 * CMenu.cpp
 *
 *  Created on: 06.04.2010
 *      Author: gerstrong
 */

#include "CMenuGalaxy.h"

#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"

#include "GraphicsIDs.h"

namespace galaxy
{

///
// Initialization
///
/**
 * \brief sets the menu up when it's opened (initialization)
 */
CMenuGalaxy::CMenuGalaxy( char menu_mode, std::string &GamePath,
		 char &Episode, CSavedGame &SavedGame,
		 stOption *pOption ) :
CMenu( menu_mode, GamePath, Episode, SavedGame, pOption, DLG_THEME_GALAXY),
processPtr(&CMenuGalaxy::processMainMenu),
m_TitleBmp(g_pGfxEngine->getBitmap(BMP_MAINMENULABEL))
{
	init(menu_mode);
}

bool CMenuGalaxy::init( char menu_type )
{
	cleanup();

	CFont &Font = g_pGfxEngine->getFont(1);
	//Font.setFGColour(g_pVideoDriver->BlitSurface->format, 0x54fc54);
	//Font.setBGColour(g_pVideoDriver->BlitSurface->format, 0x545454);
	//Font.setFGColour(g_pVideoDriver->BlitSurface->format, 0xFF0000);
	Font.setBGColour(g_pVideoDriver->BlitSurface->format, 0x545454);

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
}

/**
 * \brief sets the menu up when it's opened (initialization)
 */
/*void CMenuGalaxy::setupMenu()
{
	// Perform coordinate calculations
	m_title_coord.x = (g_pVideoDriver->getGameResolution().w - m_TitleBmp.getWidth())/2-45;
	m_title_coord.y = 48;

	mp_Dialog = new CDialog(23, 10, 'u', DLG_THEME_NONE);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,1,3,"Quit");
}*/

///
// Processes
///
/**
 * \brief This cycle is processed every LPS. It's the menu core process
 */
void CMenuGalaxy::process()
{
	/*CFont &Font = g_pGfxEngine->getFont(1);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Hello Hardcore Keener!",80,65);
	Font.drawFont(g_pVideoDriver->BlitSurface,"I'm happy that you're trying",80,73);
	Font.drawFont(g_pVideoDriver->BlitSurface,"to play our Galaxy Engine.",80,81);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Sorry, but it's not yet",80,99);
	Font.drawFont(g_pVideoDriver->BlitSurface,"finished.",80,107);
	Font.drawFont(g_pVideoDriver->BlitSurface,"As you might have seen",80,115);
	Font.drawFont(g_pVideoDriver->BlitSurface,"we are doing our best",80,123);
	Font.drawFont(g_pVideoDriver->BlitSurface,"to get it running.",80,131);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Regards, The CG Team",80,139);

	Font.setBGColour(g_pVideoDriver->FGLayerSurface->format, 0x545454);*/

	// Process the Menu Type logic.
	CMenu::process();

	if(processPtr != NULL)
		(this->*processPtr)();

	// Only if a selection has been done, trigger it!
	if(m_selection != -1 && !m_goback)
	{
		for( std::size_t iter = 0 ; iter < m_menumap.size() ; iter++ )
		{
			if( m_selection == static_cast<int>(iter) )
			{
				init( m_menumap.at(iter) );
				break;
			}
		}
	}

	if(m_goback && m_menu_type != MAIN)
		init(m_menuback[m_menu_type]);
}

/**
 * \brief This is the child process only for the main menu itself
 */
void CMenuGalaxy::processMainMenu()
{
	m_TitleBmp.draw(g_pVideoDriver->BlitSurface, m_title_coord.x, m_title_coord.y);

	if( m_selection != -1)
	{
		if( m_menu_mode == PASSIVE )
		{
			if( m_selection == 5 ) // Back to Demo
				m_demoback = true;

			else if( m_selection == 6 ) // Choose Game
				m_choosegame = true;
		}
		else if( m_menu_mode == ACTIVE )
		{
			if( m_selection == 5 ) // Back to Game
				m_goback = true;
		}
	}
	if( m_menu_mode == PASSIVE )
	{
		if(m_goback)
			init(QUIT);
	}

	if( m_selection == 3 ) // Show Highscores
	{
		// TODO: Code here will a special case and must be done!
	}
}

CMenuGalaxy::~CMenuGalaxy()
{
	delete mp_Dialog;
}

}
