/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"
#include "CMapLoaderGalaxy.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"

namespace galaxy {

CWorldMap::CWorldMap(CExeFile &ExeFile):
m_ExeFile(ExeFile)
{}

bool CWorldMap::isActive()
{	return m_active;	}

void CWorldMap::setActive(bool value)
{	m_active = value;	}

void CWorldMap::init()
{
	// Load the World map level.
	CMapLoaderGalaxy MapLoader(m_ExeFile);

	m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());
	MapLoader.loadMap(m_Map,0); // Map Level?

	m_Map.drawAll();
}

void CWorldMap::process()
{
	processInput();

	// Animate the tiles of the map
	m_Map.animateAllTiles();
}

void CWorldMap::processInput()
{
	// TODO: wrap this into the camera class they way it can be used in the vorticons engine
	// Also make it more efficient.

	//if(g_pInput->getPressedCommand(IC_QUIT))
		//m_endgame = true;

	if(g_pInput->getHoldedCommand(IC_LEFT))
	{
		m_Map.scrollLeft();
		m_Map.scrollLeft();
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT))
	{
		m_Map.scrollRight();
		m_Map.scrollRight();
	}
	if(g_pInput->getHoldedCommand(IC_UP))
	{
		m_Map.scrollUp();
		m_Map.scrollUp();
	}
	else if(g_pInput->getHoldedCommand(IC_DOWN))
	{
		m_Map.scrollDown();
		m_Map.scrollDown();
	}
}

CWorldMap::~CWorldMap() {
	// TODO Auto-generated destructor stub
}

}
