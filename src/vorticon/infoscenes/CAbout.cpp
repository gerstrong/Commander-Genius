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
	mp_Map->gotoPos( 1008, 28 );
	
	// Load the SDL_Bitmap
	if(type == "ID")
	{
		mp_bmp = g_pGfxEngine->getBitmap("IDLOGO");
		CExeFile *Exefile = new CExeFile(episode, datadirectory);
		
		// load the exe file
		Exefile->readData();
		
		// Get the offset where in the data the info is...
		size_t offset = 0;
		size_t offsetend = 0;
		switch(episode)
		{
			case 1:
				m_numberoflines = 13; // numberof lines to print
				if(Exefile->getEXEVersion() == 131)
					offset = 0x16180-512;
				break;
			case 2:
								m_numberoflines = 13; // numberof lines to print
				if(Exefile->getEXEVersion() == 131)
					offset = 0x1A954-512;
				break;
			case 3:
								m_numberoflines = 13; // numberof lines to print
				if(Exefile->getEXEVersion() == 131)
					offset = 0x1CA70-512;
				break;
		}
		mp_Map->drawAll();
		
		// Read the strings and save them the string array of the class
		if(offset)
		{
			char *data;
			data = (char*)Exefile->getData() + offset;
			std::string buf;
			for(int i=0 ; i<m_numberoflines ; i++)
			{
				if(*data == '\0')
				{
					data++;
					while(*data == '\0')
						data++;
				}
				while(*data != '\n' and *data != '\0') // For the next line
				{
					buf.push_back(*data);
					data++;
				}
				data++;
				m_lines.push_back(buf);
				
				buf.clear();
			}
		}
		
		delete Exefile;
	}
	else if(type == "CG")
	{
		std::string path = GetFullFileName("res/gfx/CGLogo.bmp");
		mp_LogoBMP = SDL_LoadBMP(path.c_str());
		
		m_lines.push_back("Commander Genius is an interpreter");
		m_lines.push_back("made with the goal of recreating");
		m_lines.push_back("the engine that was used to power");
		m_lines.push_back("the Commander Keen series.");
		m_lines.push_back("");
		m_lines.push_back("However, we are also trying to add");
		m_lines.push_back("better support for modern systems");
		m_lines.push_back("to the games, so they can run more");
		m_lines.push_back("smoothly than they did in DOS.");
		m_lines.push_back("");
		m_lines.push_back("Thank you for supporting us by");
		m_lines.push_back("downloading Commander Genius and");
		m_lines.push_back("we hope you will report any bugs.");
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
		m_logo_rect.y = 22;
	}
}


void CAbout::process()
{	 
	mp_Map->animateAllTiles();
	
	
	if(m_type == "ID")
	{
		mp_bmp->draw( g_pVideoDriver->FGLayerSurface, 160-mp_bmp->getWidth()/2, 22);
		
		for(std::size_t i=0 ; i<m_lines.size() ; i++)
			g_pGfxEngine->Font->drawFont(g_pVideoDriver->FGLayerSurface, m_lines[i], 24, 72+i*8, LETTER_TYPE_RED);
	}
	else if(m_type == "CG")
	{
		if(mp_LogoBMP)
			SDL_BlitSurface(mp_LogoBMP, NULL, g_pVideoDriver->FGLayerSurface, &m_logo_rect);
		
		for(std::size_t i=0 ; i<m_lines.size() ; i++)
			g_pGfxEngine->Font->drawFont(g_pVideoDriver->FGLayerSurface, m_lines[i], 24, 72+i*8, LETTER_TYPE_RED);
	}
	
	if(g_pInput->getPressedAnyKey())
		m_destroy_me=true;
}

CAbout::~CAbout() {
	if(mp_LogoBMP) SDL_FreeSurface(mp_LogoBMP);
}
