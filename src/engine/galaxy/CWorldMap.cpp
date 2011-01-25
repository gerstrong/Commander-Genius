/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"
#include "CMapLoaderGalaxy.h"
#include "sdl/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/sound/CMusic.h"
#include "StringUtils.h"

namespace galaxy {

CWorldMap::CWorldMap(CExeFile &ExeFile,
		CInventory &Inventory, stOption *p_option):
m_active(false),
m_ExeFile(ExeFile),
m_Inventory(Inventory),
mp_option(p_option)
{}

bool CWorldMap::isActive()
{	return m_active;	}

void CWorldMap::setActive(bool value)
{
	m_active = value;
	if(m_active)
	{
		m_Map.drawAll();
		m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());
	    // Set Scrollbuffer
	    g_pVideoDriver->setScrollBuffer(&m_Map.m_scrollx_buf, &m_Map.m_scrolly_buf);
	}
}

void CWorldMap::init()
{
	// Load the World map level.
	CMapLoaderGalaxy MapLoader(m_ExeFile, m_ObjectPtr, m_Inventory);

	m_Map.setScrollSurface(g_pVideoDriver->getScrollSurface());
	MapLoader.loadMap(m_Map, 0); // Is it a Map Level?

	m_Map.drawAll();
}

/**
 * The map is not loaded again after the game started. But we have to reload its song.
 */
void CWorldMap::loadAndPlayMusic()
{
	g_pMusicPlayer->stop();
	if(g_pMusicPlayer->LoadFromAudioCK(m_ExeFile, 0));
		g_pMusicPlayer->play();
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

	// Draw masked tiles here!
	m_Map.drawForegroundTiles();

	// Draw the inventory
	if(mp_option[OPT_HUD].value)
		m_Inventory.drawHUD();
}


CWorldMap::~CWorldMap() {
	while(!m_ObjectPtr.empty())
	{
		delete m_ObjectPtr.back();
		m_ObjectPtr.pop_back();
	}
}

}
