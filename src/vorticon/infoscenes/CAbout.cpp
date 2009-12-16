/*
 * CAbout.cpp
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#include "CAbout.h"
#include "../../FindFile.h"
#include "../../sdl/CInput.h"
#include "../../CLogFile.h"
#include "../../fileio/CExeFile.h"
#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../common/CMapLoader.h"

CAbout::CAbout(std::string &datadirectory, char &episode, const std::string& type) :
mp_LogoBMP(NULL),
m_type(type)
{
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	mp_Map = new CMap(mp_Scrollsurface, g_pGfxEngine->Tilemap);
	CMapLoader Maploader(mp_Map);
	
	Maploader.load(episode, 90, datadirectory);
	mp_Map->gotoPos( 22<<4, 32 );

	// Load the SDL_Bitmap
	if(type == "CG")
	{
		std::string path = GetFullFileName("res/CGLogo.bmp");
		mp_LogoBMP = SDL_LoadBMP(path.c_str());

		m_lines.push_back("Commander Genius is an interpreter");
		m_lines.push_back("which tries to make the");
		m_lines.push_back("Commander Keen series playable.");
		m_lines.push_back("Different than an emulator");
		m_lines.push_back("it does not replay old hardware,");
		m_lines.push_back("it tries to simulate");
		m_lines.push_back("the game behavior.");
		m_lines.push_back("We also try to support mods as");
		m_lines.push_back("there are great ones out there!");
	}

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

	m_logo_rect.x = m_logo_rect.y = 0;
	m_logo_rect.h = m_logo_rect.w = 0;

	if(mp_LogoBMP)
	{
		m_logo_rect.w = mp_LogoBMP->w;
		m_logo_rect.h = mp_LogoBMP->h;
		m_logo_rect.x = 160-m_logo_rect.w/2;
		m_logo_rect.y = 50;
	}
}


void CAbout::process()
{	 
	mp_Map->animateAllTiles();
	
	if(m_type == "CG")
	{
		if(mp_LogoBMP)
			SDL_BlitSurface(mp_LogoBMP, NULL, g_pVideoDriver->FGLayerSurface, &m_logo_rect);

		for(std::size_t i=0 ; i<m_lines.size() ; i++)
			g_pGfxEngine->Font->drawFont(g_pVideoDriver->FGLayerSurface, m_lines[i], 160-m_lines[i].size()*4, 100+i*8, LETTER_TYPE_RED);
	}

	if(g_pInput->getPressedAnyKey())
		m_destroy_me=true;
}

CAbout::~CAbout() {
	if(mp_LogoBMP) SDL_FreeSurface(mp_LogoBMP);
}
