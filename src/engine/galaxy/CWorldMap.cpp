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
#include "../../StringUtils.h"

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
	CMapLoaderGalaxy MapLoader(m_ExeFile, m_ObjectPtr);

	m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());
	MapLoader.loadMap(m_Map,0); // Map Level?

	m_Map.drawAll();
}

void CWorldMap::process()
{
	processInput();

	// Animate the tiles of the map
	m_Map.animateAllTiles();


	for( std::vector<CObject*>::iterator obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		(*obj)->process();
	}

	g_pVideoDriver->blitScrollSurface();

	for( std::vector<CObject*>::iterator obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		(*obj)->draw();
	}

	std::string text1, text2, text3;

	text1 = "Obj1- X:" + itoa((*m_ObjectPtr.begin())->getXPosition());
	text1 += " Y:" + itoa((*m_ObjectPtr.begin())->getYPosition());
	text2 = "Obj2- X:" + itoa((*m_ObjectPtr.begin())->getXPosition()+1);
	text2 += " Y:" + itoa((*m_ObjectPtr.begin())->getYPosition()+1);
	text3 = "Edge (" + itoa(m_Map.m_width<<CSF)+","+itoa(m_Map.m_height<<CSF) + ")";

	g_pGfxEngine->getFont(0).drawFont(g_pVideoDriver->BlitSurface, text1, 10, 10);
	g_pGfxEngine->getFont(0).drawFont(g_pVideoDriver->BlitSurface, text2, 10, 18);
	g_pGfxEngine->getFont(0).drawFont(g_pVideoDriver->BlitSurface, text3, 10, 26);
}

void CWorldMap::processInput()
{
	// TODO: wrap this into the camera class they way it can be used in the vorticons engine
	// Also make it more efficient.

	//if(g_pInput->getPressedCommand(IC_QUIT))
		//m_endgame = true;

}

CWorldMap::~CWorldMap() {
	while(!m_ObjectPtr.empty())
	{
		delete m_ObjectPtr.back();
		m_ObjectPtr.pop_back();
	}
}

}
