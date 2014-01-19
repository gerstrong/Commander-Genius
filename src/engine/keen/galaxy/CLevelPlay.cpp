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
//#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "graphics/effects/CDimDark.h"
#include <lib/base/GsLogging.h>

#include "CMapPlayGalaxy.h"

#include "GalaxyEngine.h"

namespace galaxy {

CLevelPlay::CLevelPlay(CExeFile &ExeFile,
        std::vector<CInventory> &inventory,
		stCheat &Cheatmode) :
CMapPlayGalaxy(ExeFile, inventory, Cheatmode)
{ }


void CLevelPlay::loadMap(const int level)
{
	// Load the World map level.
	std::unique_ptr<CMapLoaderGalaxy> MapLoader;

	if(g_pBehaviorEngine->getEpisode() == 4)
        MapLoader.reset(new CMapLoaderGalaxyEp4(mExeFile, mObjectPtr, mInventoryVec, mCheatmode));
	else if(g_pBehaviorEngine->getEpisode() == 5)
        MapLoader.reset(new CMapLoaderGalaxyEp5(mExeFile, mObjectPtr, mInventoryVec, mCheatmode));
	else if(g_pBehaviorEngine->getEpisode() == 6)
        MapLoader.reset(new CMapLoaderGalaxyEp6(mExeFile, mObjectPtr, mInventoryVec, mCheatmode));

	MapLoader->loadMap( mMap, level );

    // Load the Background Music
	g_pMusicPlayer->stop();

    if( loadLevelMusic(level) )
    	gLogging.textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    else
    	g_pMusicPlayer->play();
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
	const std::string loading_text = g_pBehaviorEngine->getString(level_text);

    gEffectController.setupEffect(new CDimDark(8));
    gEventManager.add( new EventSendBitmapDialogMsg(*gGraphics.getBitmapFromStr("KEENTHUMBSUP"),
							  loading_text, LEFT) );

	mMap.drawAll();
	return true;
}


}
