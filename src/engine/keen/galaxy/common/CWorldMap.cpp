/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"
#include "fileio/CSaveGameController.h"
#include "graphics/effects/CColorMerge.h"
#include "engine/core/VGamepads/vgamepadsimple.h"

#include "../GalaxyEngine.h"
#include "dialog/CMessageBoxBitmapGalaxy.h"

#include <base/video/CVideoDriver.h>

namespace galaxy {

CWorldMap::CWorldMap(std::vector<CInventory> &inventoryVec):
CMapPlayGalaxy(inventoryVec)
{}

bool CWorldMap::init()
{
	// Load the World map level.
	std::unique_ptr<CMapLoaderGalaxy> MapLoader;

	if(gBehaviorEngine.getEpisode() == 4)
    {
        MapLoader.reset( new CMapLoaderGalaxyEp4( mObjectPtr, mInventoryVec) );
    }
	else if(gBehaviorEngine.getEpisode() == 5)
    {
        MapLoader.reset( new CMapLoaderGalaxyEp5( mObjectPtr, mInventoryVec) );
    }
	else if(gBehaviorEngine.getEpisode() == 6)
    {
        MapLoader.reset( new CMapLoaderGalaxyEp6( mObjectPtr, mInventoryVec) );
    }

    const bool ok = MapLoader->loadMap( mMap, 0 );

    if(!ok)
    {
        return false;
    }

	gBehaviorEngine.mapLevelName = MapLoader->getLevelName();

    const std::string loading_text = gBehaviorEngine.getString("LEVEL0_LOAD_TEXT");

    gEffectController.setupEffect(new CColorMerge(8));
	
	if(!gSaveGameController.busy())
	{
        showMsgWithBmp(loading_text, "KEENTHUMBSUP", LEFT);
	}

	mMap.drawAll();

    return true;
}


/**
 * The map is not loaded again after the game started. But we have to reload its song.
 */
void CWorldMap::loadAndPlayMusic()
{
	gMusicPlayer.stop();
    if(loadLevelMusic(0))
    {
		gMusicPlayer.play();
    }
}

void CWorldMap::ponder(const float deltaT)
{

    if( gVideoDriver.VGamePadEnabled() )
    {
        VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
        assert(vkc);
        vkc->mButtonMode = VirtualKeenControl::WMAP;
        vkc->mHideStartButton = true;
    }

    ponderBase(deltaT);
}


}
