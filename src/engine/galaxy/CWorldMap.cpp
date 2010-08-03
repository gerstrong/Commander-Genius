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
m_active(false),
m_ExeFile(ExeFile),
m_newLevel(false)
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
	MapLoader.loadMap(m_Map, 0); // Map Level?

	m_Map.drawAll();
}

void CWorldMap::process()
{
	// Animate the tiles of the map
	m_Map.animateAllTiles();

	for(size_t i=0 ; i<m_ObjectPtr.size() ; i++)
	{
		CObject* p_Object = m_ObjectPtr[i];

		p_Object->process();
	}

	g_pVideoDriver->blitScrollSurface();

	for( std::vector<CObject*>::iterator obj=m_ObjectPtr.begin() ;
			obj!=m_ObjectPtr.end() ; obj++ )
	{
		(*obj)->draw();
	}

}


CWorldMap::~CWorldMap() {
	while(!m_ObjectPtr.empty())
	{
		delete m_ObjectPtr.back();
		m_ObjectPtr.pop_back();
	}
}

}
