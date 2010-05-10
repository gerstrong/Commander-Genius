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
CMenu( menu_mode, GamePath, Episode, SavedGame, pOption ),
processPtr(&CMenuGalaxy::processMainMenu),
m_TitleBmp(g_pGfxEngine->getBitmap(BMP_MAINMENULABEL))
{
	setupMenu();
}

/**
 * \brief sets the menu up when it's opened (initialization)
 */
void CMenuGalaxy::setupMenu()
{
	// Perform coordinate calculations
	m_title_coord.x = (g_pVideoDriver->getGameResolution().w - m_TitleBmp.getWidth())/2-45;
	m_title_coord.y = 48;

	mp_Dialog = new CDialog(23, 10);

	// Use the standard Menu-Frame used in the old DOS-Games
	mp_Dialog->setFrameTheme( DLG_THEME_NONE );
	mp_Dialog->setFontID(1);
	mp_Dialog->setGalaxyStyle(true);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "New Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Load Game");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT,1,3,"Quit");
}

///
// Processes
///
/**
 * \brief This cycle is processed every LPS. It's the menu core process
 */
void CMenuGalaxy::process()
{
	CFont &Font = g_pGfxEngine->getFont(1);

	Font.setFGColour(g_pVideoDriver->BlitSurface->format, 0x54fc54);
	Font.setBGColour(g_pVideoDriver->BlitSurface->format, 0x545454);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Hello Hardcore Keener!",80,65);
	Font.drawFont(g_pVideoDriver->BlitSurface,"I'm happy that you're trying",80,73);
	Font.drawFont(g_pVideoDriver->BlitSurface,"to play our Galaxy Engine.",80,81);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Sorry, but it's not yet",80,99);
	Font.drawFont(g_pVideoDriver->BlitSurface,"finished.",80,107);
	Font.drawFont(g_pVideoDriver->BlitSurface,"As you might have seen",80,115);
	Font.drawFont(g_pVideoDriver->BlitSurface,"we are doing our best",80,123);
	Font.drawFont(g_pVideoDriver->BlitSurface,"to get it running.",80,131);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Regards, The CG Team",80,139);

	//mp_Dialog->processInput();

	(this->*processPtr)();

	Font.setBGColour(g_pVideoDriver->FGLayerSurface->format, 0x545454);
	//mp_Dialog->draw();
}

/**
 * \brief This is the child process only for the main menu itself
 */
void CMenuGalaxy::processMainMenu()
{
	m_TitleBmp.draw(g_pVideoDriver->BlitSurface, m_title_coord.x, m_title_coord.y);

	// TODO: Code for the main menu goes here!
}

CMenuGalaxy::~CMenuGalaxy()
{
	delete mp_Dialog;
}

}
