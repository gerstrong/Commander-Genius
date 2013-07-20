/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"
#include "fileio/CSaveGameController.h"
#include "graphics/effects/CColorMerge.h"

namespace galaxy {

CWorldMap::CWorldMap(CExeFile &ExeFile,
					CInventory &Inventory,
					stCheat &Cheatmode):
CMapPlayGalaxy(ExeFile, Inventory, Cheatmode)
{}

void CWorldMap::init()
{
	// Load the World map level.
	std::unique_ptr<CMapLoaderGalaxy> MapLoader;

	if(g_pBehaviorEngine->getEpisode() == 4)
		MapLoader.reset( new CMapLoaderGalaxyEp4(mExeFile, mObjectPtr, mInventory, mCheatmode) );
	else if(g_pBehaviorEngine->getEpisode() == 5)
		MapLoader.reset( new CMapLoaderGalaxyEp5(mExeFile, mObjectPtr, mInventory, mCheatmode) );
	else if(g_pBehaviorEngine->getEpisode() == 6)
		MapLoader.reset( new CMapLoaderGalaxyEp6(mExeFile, mObjectPtr, mInventory, mCheatmode) );

	MapLoader->loadMap( mMap, 0 );
	g_pBehaviorEngine->mapLevelName = MapLoader->getLevelName();

	const std::string loading_text = g_pBehaviorEngine->getString("WORLDMAP_LOAD_TEXT");
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	g_pGfxEngine->setupEffect(new CColorMerge(8));
	
	if(!gpSaveGameController->busy())
	{
	    EventContainer.add( new EventSendBitmapDialogMsg(*g_pGfxEngine->getBitmap("KEENTHUMBSUP"), loading_text, LEFT) );
	}

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
