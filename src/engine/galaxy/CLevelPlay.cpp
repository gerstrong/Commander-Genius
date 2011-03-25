/*
 * CLevelPlay.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CLevelPlay.h"
#include "CMapLoaderGalaxy.h"
#include "sdl/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "CLogFile.h"

#include "CMapPlayGalaxy.h"

namespace galaxy {

CLevelPlay::CLevelPlay(CExeFile &ExeFile,
		CInventory &Inventory) :
CMapPlayGalaxy(ExeFile, Inventory)
{ }


bool CLevelPlay::loadLevel(const Uint16 level)
{
	// Load the World map level.
	CMapLoaderGalaxy MapLoader(m_ExeFile, m_ObjectPtr, m_Inventory);

	MapLoader.loadMap(m_Map, level);

    // Load the Background Music
	g_pMusicPlayer->stop();

    if(!g_pMusicPlayer->load(m_ExeFile, level))
    	g_pLogFile->textOut("Warning: The music cannot be played. Check that all the files have been correctly copied!");
    else
    	g_pMusicPlayer->play();

	m_Map.drawAll();
	return true;
}

}
