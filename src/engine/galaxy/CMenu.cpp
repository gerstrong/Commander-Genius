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
	m_title_coord.x = (g_pVideoDriver->getGameResolution().w - m_TitleBmp.getWidth())/2-5;
	m_title_coord.y = 48;
}

///
// Processes
///
void CMenu::process()
{
	(this->*processPtr)();
}

void CMenu::processMainMenu()
{
	m_TitleBmp.draw(g_pVideoDriver->BlitSurface, m_title_coord.x, m_title_coord.y);
}

CMenu::~CMenu()
{
	// TODO Auto-generated destructor stub
}

}
