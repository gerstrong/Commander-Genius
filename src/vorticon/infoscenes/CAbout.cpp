/*
 * CAbout.cpp
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#include "../../keen.h"
#include "CAbout.h"
#include "../../sdl/CInput.h"
#include "../../CLogFile.h"
#include "../../include/gamedo.h"
#include "../../fileio/CExeFile.h"
#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../common/CMapLoader.h"

CAbout::CAbout(std::string &datadirectory, char &episode, std::string type) {
	//CExeFile *Exefile = new CExeFile(episode, datadirectory);
	// Commented out, because it's unused
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	mp_Map = new CMap(mp_Scrollsurface, g_pGfxEngine->Tilemap);
	CMapLoader Maploader(mp_Map);
	
	Maploader.load(episode, 90, datadirectory);
	mp_Map->gotoPos( 22<<4, 32 );
	
}


void CAbout::process()
{	 
	mp_Map->animateAllTiles();
	
	if(g_pInput->getPressedAnyKey())
		m_destroy_me=true;
}

CAbout::~CAbout() {
	// TODO Auto-generated destructor stub
}
