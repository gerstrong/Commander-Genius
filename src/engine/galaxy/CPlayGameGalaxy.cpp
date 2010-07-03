/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"
#include "CMapLoaderGalaxy.h"

#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"
#include "../../graphics/CGfxEngine.h"
#include "../../StringUtils.h"

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, char difficulty, stOption *p_option) :
CPlayGame(ExeFile, level, numplayers, difficulty, p_option),
m_posx(0),
m_posy(0)
{}

bool CPlayGameGalaxy::loadGameState()
{return false;}

// Setup for the ingame
bool CPlayGameGalaxy::init()
{
	loadLevel();

	return false;
}

void CPlayGameGalaxy::loadLevel()
{
	// Load the Level map. We have two modes. Inlevel and game map

	// TODO: Lets load the main map for now and create process for this
	CMapLoaderGalaxy MapLoader(m_ExeFile);

	m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());

	MapLoader.loadMap(m_Map, m_Level); // Map Level?

	m_Map.drawAll();
	m_Map.gotoPos(0, 0); // Coordinates of star sky
}

// The main ingame process cycle when keen galaxy is up and running
void CPlayGameGalaxy::process()
{
	// TODO: Process code for the main map
	// Blit the background

	processInput();

	processRendering();
}

void CPlayGameGalaxy::processInput()
{
	if(g_pInput->getPressedCommand(IC_QUIT))
		m_endgame = true;

	if(g_pInput->getHoldedCommand(IC_LEFT))
		m_Map.scrollLeft();
	else if(g_pInput->getHoldedCommand(IC_RIGHT))
		m_Map.scrollRight();

	if(g_pInput->getHoldedCommand(IC_UP))
		m_Map.scrollUp();
	else if(g_pInput->getHoldedCommand(IC_DOWN))
		m_Map.scrollDown();

	if(g_pInput->getPressedCommand(IC_STATUS))
	{
		if(m_Level < 20)
			m_Level++;
		else
			m_Level = 0;
		loadLevel();
	}
}

void CPlayGameGalaxy::processRendering()
{
	// Animate the tiles of the map
	m_Map.animateAllTiles();

	// Blit the background
	g_pVideoDriver->blitScrollSurface();

	g_pGfxEngine->getFont(0).drawFont(g_pVideoDriver->BlitSurface, "Press the arrows to scroll, ESC to quit", 10, 10);
	g_pGfxEngine->getFont(0).drawFont(g_pVideoDriver->BlitSurface, "Enter to switch Level", 10, 20);
}


void CPlayGameGalaxy::cleanup()
{}

CPlayGameGalaxy::~CPlayGameGalaxy()
{
	// TODO Auto-generated destructor stub
}

}
