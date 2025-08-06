/*
 * CLevelPlay.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include <memory>

#include "CLevelPlay.h"
#include "ep4/CMapLoaderGalaxyEp4.h"
#include "ep5/CMapLoaderGalaxyEp5.h"
#include <base/CInput.h>
#include <base/audio/music/CMusic.h>
#include <graphics/CDimDark.h>
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>

#include "engine/core/VGamepads/vgamepadsimple.h"

#include "CMapPlayGalaxy.h"

#include "GalaxyEngine.h"
#include "common/dialog/CMessageBoxBitmapGalaxy.h"

namespace galaxy {

CLevelPlay::CLevelPlay(std::vector<CInventory> &inventory) :
CMapPlayGalaxy(inventory)
{ }


bool CLevelPlay::loadMap(const int level)
{      
	// Load the World map level.
	std::unique_ptr<CMapLoaderGalaxy> MapLoader;

    const int episode = gBehaviorEngine.getEpisode();

    if(episode == 4)
    {
        MapLoader.reset(new CMapLoaderGalaxyEp4(mObjectPtr, mInventoryVec));
    }
    else if(episode == 5)
    {
        MapLoader.reset(new CMapLoaderGalaxyEp5(mObjectPtr, mInventoryVec));
    }
    else if(episode == 6)
    {
        MapLoader.reset(new CMapLoaderGalaxyEp6(mObjectPtr, mInventoryVec, gBehaviorEngine.isDemo()));
    }
    else
    {
        gLogging.textOut("Warning: Episode number does not make sense");
    }


    if(!MapLoader->loadMap( mpMap, level ))
    {
        gLogging.ftextOut("Error: Map of level %d could not be loaded.", level);
        return false;
    }


    // Load some new background Music
    gMusicPlayer.stop();
    if( galaxy::loadLevelMusic(level) )
    {
        mCurMusicTrack = gMusicPlayer.getCurTrack();
        gMusicPlayer.play();
    }
    else
    {
        gLogging.textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    }

    return true;
}


void CLevelPlay::reloadLevel()
{
    // Create the special merge effect (Fadeout)
    auto dimDark = new CDimDark(8);
    dimDark->getSnapshot();
    gEffectController.setupEffect(dimDark);

    loadMap( mpMap->getLevel() );
}

bool CLevelPlay::loadLevel(const int sprVar, const Uint16 level)
{
    if(!loadMap( level ))
    {
        return false;
    }

	// Add the load message
	const std::string level_text = "LEVEL" + itoa(level) + "_LOAD_TEXT";
    const std::string loading_text = gBehaviorEngine.getString(level_text);

    showModalMsgWithBmp( sprVar, loading_text, "KEENTHUMBSUPLOADING",
                    LEFT, true, nullptr );

    mpMap->drawAll();

	return true;
}

void CLevelPlay::ponder(const float deltaT)
{    
#ifdef USE_VIRTUALPAD
#if SDL_VERSION_ATLEAST(2, 0, 0)
    if( gVideoDriver.VGamePadEnabled() )
    {
        VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
        assert(vkc);
        vkc->hideAllButtons();
        vkc->mDPad.invisible = false;
        vkc->mJumpButton.invisible = false;
        vkc->mPogoButton.invisible = false;
        vkc->mShootButton.invisible = false;
        vkc->mStatusButton.invisible = false;
        vkc->mMenuButton.invisible = false;
    }
#endif
#endif

    ponderBase(deltaT);
}



}
