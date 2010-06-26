/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"
#include "CMapLoaderGalaxy.h"

#include "../../sdl/CVideoDriver.h"
#include "../../graphics/CGfxEngine.h"

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, char difficulty, stOption *p_option) :
CPlayGame(ExeFile, level, numplayers, difficulty, p_option)
{}

bool CPlayGameGalaxy::loadGameState()
{return false;}

// Setup for the ingame
bool CPlayGameGalaxy::init()
{
	// Load the Level map. We have two modes. Inlevel and game map

	// TODO: Lets load the main map for now and create process for this
	CMapLoaderGalaxy MapLoader(m_ExeFile);

	m_Map.setTileMap(g_pGfxEngine->getTileMap(0));
	m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());

	MapLoader.loadMap(m_Map, 2); // Map Level?

	m_Map.gotoPos(0, 0); // Coordinates of star sky
	//m_Map.drawAll();

	// We should create this as base and two more classes. One will be for ingame and the other for
	// the map processes

	return false;
}


// The main ingame process cycle when keen galaxy is up and running
void CPlayGameGalaxy::process()
{
	// TODO: Process code for the main map
	// Blit the background

	//g_pVideoDriver->blitScrollSurface();
	for(size_t x=0 ; x<20 ; x++)
	{
		for(size_t y=0 ; y<20 ; y++)
		{
			g_pGfxEngine->getTileMap(0).drawTile(g_pVideoDriver->getBlitSurface(),
				16*x, 16*y, m_Map.at((x+2),(y+2),0) );
		}
	}
}

void CPlayGameGalaxy::cleanup()
{}

CPlayGameGalaxy::~CPlayGameGalaxy()
{
	// TODO Auto-generated destructor stub
}

}
