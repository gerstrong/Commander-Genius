/*
 * CPlayGame.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPlayGameGalaxy.h"
#include "CMapLoaderGalaxy.h"

#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../StringUtils.h"

namespace galaxy
{

CPlayGameGalaxy::CPlayGameGalaxy(CExeFile &ExeFile, char level,
		 char numplayers, char difficulty, stOption *p_option) :
CPlayGame(ExeFile, level, numplayers, difficulty, p_option),
m_WorldMap(ExeFile)
{
	m_WorldMap.init();
}

bool CPlayGameGalaxy::loadGameState()
{	return false;	}

// Setup for the ingame
bool CPlayGameGalaxy::init()
{
	loadLevel();

	return false;
}

void CPlayGameGalaxy::loadLevel()
{
	// Load the Level map. We have two modes. Inlevel and game map

	// TODO: Lets load the main map for now and create a process for this
	//CMapLoaderGalaxy MapLoader(m_ExeFile);

	//m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());
	//MapLoader.loadMap(m_Map,0); // Map Level?

	//m_Map.drawAll();
}

// The main ingame process cycle when keen galaxy is up and running
void CPlayGameGalaxy::process()
{
	// TODO: Process code for the main map
	// Blit the background

	processInput();

	// process World Map if active. At the start it's enabled
	if(m_WorldMap.isActive())
		m_WorldMap.process();

	// process World Map if active. At the start it's enabled
	//if(m_LevelPlay.isActive())
		//m_LevelPlay.process();

	// process Page if one is open. Could be one of the finale
	//if(m_Page.isActive())
		//m_Page.process();

	processRendering();
}

void CPlayGameGalaxy::processInput()
{

}

/*
 * This processes the rendering over all the other stuff. THis is used for extra messages normally
 * blit scrollsurface happens with m_WorldMap or m_LevelPlay
 */
void CPlayGameGalaxy::processRendering()
{

}


void CPlayGameGalaxy::cleanup()
{}

CPlayGameGalaxy::~CPlayGameGalaxy()
{
	// TODO Auto-generated destructor stub
}

}
