/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"
#include "dialog/CMessageBoxBitmapGalaxy.h"

namespace galaxy {

CWorldMap::CWorldMap(CExeFile &ExeFile,
					CInventory &Inventory,
					stCheat &Cheatmode):
CMapPlayGalaxy(ExeFile, Inventory, Cheatmode)
{}

void CWorldMap::init()
{
	// Load the World map level.
	CMapLoaderGalaxy MapLoader(mExeFile, mObjectPtr, mInventory, mCheatmode);

	MapLoader.loadMap(mMap, 0); // Is it a Map Level?
    const std::string loading_text = g_pBehaviorEngine->getString("WORLDMAP_LOAD_TEXT");

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	EventContainer.add( new EventSendBitmapDialogMsg(106, loading_text, LEFT) );

	mMap.drawAll();
}


/**
 * The map is not loaded again after the game started. But we have to reload its song.
 */
void CWorldMap::loadAndPlayMusic()
{
	g_pMusicPlayer->stop();
	if(g_pMusicPlayer->load(mExeFile, 0))
		g_pMusicPlayer->play();
}


}
