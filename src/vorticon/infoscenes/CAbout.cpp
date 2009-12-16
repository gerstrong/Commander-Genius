/*
 * CAbout.cpp
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

//#include "../../keen.h"
#include "CAbout.h"
#include "../../sdl/CInput.h"
#include "../../CLogFile.h"
#include "../../fileio/CExeFile.h"
#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../common/CMapLoader.h"

CAbout::CAbout(std::string &datadirectory, char &episode, std::string type) :
mp_LogoBMP(NULL)
{
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	mp_Map = new CMap(mp_Scrollsurface, g_pGfxEngine->Tilemap);
	CMapLoader Maploader(mp_Map);
	
	Maploader.load(episode, 90, datadirectory);
	mp_Map->gotoPos( 22<<4, 32 );

	// Load the SDL_Bitmap
	//mp_LogoBMP = SDL_LoadBMP();

	switch(episode)
	{
		case 1:
			// Change the ugly lower Tiles which are seen, when using 320x240 base resolution
			for(int i=0; i<20 ; i++)
			{
				mp_Map->changeTile(22+i, 15, 14*13);
				mp_Map->changeTile(22+i, 16, 14*13+3);
			}
			break;
	}
}


void CAbout::process()
{	 
	mp_Map->animateAllTiles();
	
	if(mp_LogoBMP)
	{

	}

	if(g_pInput->getPressedAnyKey())
		m_destroy_me=true;
}

CAbout::~CAbout() {
	if(mp_LogoBMP) SDL_FreeSurface(mp_LogoBMP);
}
