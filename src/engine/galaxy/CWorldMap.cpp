/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"

namespace galaxy {

CWorldMap::CWorldMap(CExeFile &ExeFile,
		CInventory &Inventory):
CMapPlayGalaxy(ExeFile, Inventory)
{}

void CWorldMap::init()
{
	// Load the World map level.
	CMapLoaderGalaxy MapLoader(m_ExeFile, m_ObjectPtr, m_Inventory, m_Cheatmode);

	MapLoader.loadMap(m_Map, 0); // Is it a Map Level?

	m_Map.drawAll();
}


/**
 * The map is not loaded again after the game started. But we have to reload its song.
 */
void CWorldMap::loadAndPlayMusic()
{
	g_pMusicPlayer->stop();
	if(g_pMusicPlayer->load(m_ExeFile, 0));
		g_pMusicPlayer->play();
}

}
