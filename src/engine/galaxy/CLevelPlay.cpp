/*
 * CLevelPlay.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CLevelPlay.h"
#include "ep4/CMapLoaderGalaxyEp4.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "dialog/CMessageBoxBitmapGalaxy.h"
#include "CLogFile.h"

#include "CMapPlayGalaxy.h"

namespace galaxy {

CLevelPlay::CLevelPlay(CExeFile &ExeFile,
		CInventory &Inventory,
		stCheat &Cheatmode) :
CMapPlayGalaxy(ExeFile, Inventory, Cheatmode)
{ }


void CLevelPlay::loadMap(const int level)
{
	// Load the World map level.
	SmartPointer<CMapLoaderGalaxy> MapLoader;

	if(g_pBehaviorEngine->getEpisode() == 4)
		MapLoader = new CMapLoaderGalaxyEp4(mExeFile, mObjectPtr, mInventory, mCheatmode);
	else
		MapLoader = new CMapLoaderGalaxy(mExeFile, mObjectPtr, mInventory, mCheatmode);

	MapLoader->loadMap( mMap, level );

    // Load the Background Music
	g_pMusicPlayer->stop();

    if( !g_pMusicPlayer->load(mExeFile, level) )
    	g_pLogFile->textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    else
    	g_pMusicPlayer->play();
}


void CLevelPlay::reloadLevel()
{
	loadMap( mMap.getLevel() );
}

bool CLevelPlay::loadLevel(const Uint16 level)
{
	loadMap( level );

    // Add the load message
    const std::string level_text = "LEVEL" + itoa(level) + "_LOAD_TEXT";
    const std::string loading_text = g_pBehaviorEngine->getString(level_text);

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	EventContainer.add( new EventSendBitmapDialogMsg(106, loading_text, LEFT) );

	mMap.drawAll();
	return true;
}

}
