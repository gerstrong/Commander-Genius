/*
 * CWorldMap.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CWorldMap.h"
#include "fileio/CSaveGameController.h"
#include <graphics/CColorMerge.h>

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
        MapLoader.reset( new CMapLoaderGalaxyEp6( mObjectPtr, mInventoryVec,
                                                  gBehaviorEngine.isDemo() ) );
    }

    const bool ok = MapLoader->loadMap( mpMap, 0 );

    if(!ok)
    {
        return false;
    }

	gBehaviorEngine.mapLevelName = MapLoader->getLevelName();

    const std::string loading_text = gBehaviorEngine.getString("LEVEL0_LOAD_TEXT");

    gEffectController.setupEffect(new CColorMerge(8));
	
	if(!gSaveGameController.busy())
	{
        assert(0 < mInventoryVec.size());
        showModalMsgWithBmp(mInventoryVec[0].mSpriteVar,
                        loading_text,
                        "KEENTHUMBSUPLOADING",
                        LEFT, true, nullptr);
	}

    mpMap->drawAll();

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
        mCurMusicTrack = gMusicPlayer.getCurTrack();
		gMusicPlayer.play();
    }
}

void CWorldMap::ponder(const float deltaT)
{

#ifdef USE_VIRTUALPAD
    if( gVideoDriver.VGamePadEnabled() )
    {
        VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
        assert(vkc);
        vkc->hideAllButtons();
        vkc->mDPad.invisible = false;
        vkc->mStartButton.invisible = false;
        vkc->mMenuButton.invisible = false;
    }
#endif

    ponderBase(deltaT);
}


}
