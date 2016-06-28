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
#include "sdl/audio/music/CMusic.h"
#include "graphics/effects/CDimDark.h"
#include <base/GsLogging.h>

#include "engine/core/VGamepads/vgamepadsimple.h"

#include "CMapPlayGalaxy.h"

#include "GalaxyEngine.h"
#include "common/dialog/CMessageBoxBitmapGalaxy.h"

namespace galaxy {

CLevelPlay::CLevelPlay(std::vector<CInventory> &inventory) :
CMapPlayGalaxy(inventory)
{ }


void CLevelPlay::loadMap(const int level)
{
	// Load the World map level.
	std::unique_ptr<CMapLoaderGalaxy> MapLoader;

    const int episode = gpBehaviorEngine->getEpisode();

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
        MapLoader.reset(new CMapLoaderGalaxyEp6(mObjectPtr, mInventoryVec));
    }
    else
    {
        gLogging.textOut("Warning: Episode number does not make sense");
    }


	MapLoader->loadMap( mMap, level );

    // Load the Background Music
	g_pMusicPlayer->stop();

    if( loadLevelMusic(level) )
    {
        g_pMusicPlayer->play();
    }
    else
    {
        gLogging.textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    }
}


void CLevelPlay::reloadLevel()
{
	loadMap( mMap.getLevel() );

	// Create the special merge effect (Fadeout)
    gEffectController.setupEffect(new CDimDark(8));
}

bool CLevelPlay::loadLevel(const Uint16 level)
{
	loadMap( level );
		
	// Add the load message
	const std::string level_text = "LEVEL" + itoa(level) + "_LOAD_TEXT";
    const std::string loading_text = gpBehaviorEngine->getString(level_text);

    showMsgWithBmp( loading_text, "KEENTHUMBSUP", LEFT );

	mMap.drawAll();
	return true;
}

void CLevelPlay::ponder(const float deltaT)
{
#ifdef TOUCHCONTROLS
    VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
    assert(vkc);
    vkc->mButtonMode = VirtualKeenControl::ACTION;
    vkc->mHideEnterButton = true;
#endif

    ponderBase(deltaT);
}

}
