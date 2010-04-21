/*
 * CMenu.cpp
 *
 *  Created on: 06.04.2010
 *      Author: gerstrong
 */

#include "CMenu.h"

#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"

#include "GraphicsIDs.h"

namespace galaxy
{

///
// Initialization
///

CMenu::CMenu() :
processPtr(&CMenu::processMainMenu),
m_TitleBmp(g_pGfxEngine->getBitmap(BMP_MAINMENULABEL))
{
	setupMenu();
}

void CMenu::setupMenu()
{
	// Perform coordinate calculations
	m_title_coord.x = (g_pVideoDriver->getGameResolution().w - m_TitleBmp.getWidth())/2-45;
	m_title_coord.y = 48;

	//mp_Dialog = new CDialog(Uint16 w, Uint16 h);
}

///
// Processes
///
void CMenu::process()
{
	CFont &Font = g_pGfxEngine->getFont(1);
	m_TitleBmp.draw(g_pVideoDriver->BlitSurface, m_title_coord.x, m_title_coord.y);
	Font.setMonoColour(g_pVideoDriver->BlitSurface->format, 0x54fc54, 0x545454);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Hello Hardcore Keener!",80,65);
	Font.drawFont(g_pVideoDriver->BlitSurface,"I'm happy that you're trying",80,73);
	Font.drawFont(g_pVideoDriver->BlitSurface,"to play our Galaxy Engine.",80,81);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Sorry, but it's not yet",80,99);
	Font.drawFont(g_pVideoDriver->BlitSurface,"finished.",80,107);
	Font.drawFont(g_pVideoDriver->BlitSurface,"As you might have seen",80,115);
	Font.drawFont(g_pVideoDriver->BlitSurface,"we are doing our best",80,123);
	Font.drawFont(g_pVideoDriver->BlitSurface,"to get it running.",80,131);
	Font.drawFont(g_pVideoDriver->BlitSurface,"Regards, The CG Team",80,139);

	(this->*processPtr)();
}

void CMenu::processMainMenu()
{
	// TODO: Code for the main menu goes here!
}

CMenu::~CMenu()
{
	// TODO Auto-generated destructor stub
}

}
