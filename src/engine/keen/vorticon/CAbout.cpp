/*
 * CAbout.cpp
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#include "CAbout.h"
#include <base/utils/FindFile.h>
#include <base/CInput.h>
#include <base/GsLogging.h>
#include "fileio/CExeFile.h"
#include "graphics/GsGraphics.h"
#include <base/video/CVideoDriver.h>
#include "CVorticonMapLoader.h"
#include "fileio/ResourceMgmt.h"
#include "sdl/extensions.h"

#include <fileio/KeenFiles.h>

CAbout::CAbout(const std::string &type) :
m_type(type)
{}

void CAbout::init()
{
    CInfoScene::init();
    CExeFile &ExeFile = gKeenFiles.exeFile;
	mpMap.reset(new CMap);
	CVorticonMapLoaderBase Maploader(mpMap);
	
    Maploader.load(ExeFile.getEpisode(), 90, gKeenFiles.gameDir);
	mpMap->gotoPos( 1008, 28 );
	
	// Load the SDL_Bitmap
	if(m_type == "ID")
	{
        mp_bmp = gGraphics.getBitmapFromStr("IDLOGO");
		
		// Get the offset where in the data the info is...
		size_t offset = 0;
		//m_numberoflines = 11;
		switch(ExeFile.getEpisode())
		{
			case 1:
				if(ExeFile.getEXEVersion() == 131)
					offset = 0x16180-512;
				break;
			case 2:
				if(ExeFile.getEXEVersion() == 131)
					offset = 0x1A954-512;
				break;
			case 3:
				if(ExeFile.getEXEVersion() == 131)
					offset = 0x1CA70-512;
				break;
		}
		mpMap->drawAll();
		
		// Read the strings and save them the string array of the class
		if(offset)
		{
			char *startdata;
			startdata = (char*)ExeFile.getRawData() + offset;
			std::string buf;
			for(int i=0 ; i<m_numberoflines ; i++)
			{
				char *data = startdata;

				for(short offset = 0 ; offset<0x28 ; offset++)
				{
					if(data[offset] == 0x0A && data[offset+1] == 0x00)
						break;

					buf.push_back(data[offset]);
				}
				startdata += 0x28;

				// now check how many new lines we have in buf
				size_t num_newlines = 0;
				bool endoftext = false;

				size_t  pos;
				if((pos = buf.find(0xFE)) != std::string::npos)
				{
					buf.erase(pos), endoftext = true;
				}

				while((pos = buf.find(0x0A)) != std::string::npos)
					buf.erase(pos,1), num_newlines++;

				while((pos = buf.find('\0')) != std::string::npos)
					buf.erase(pos,1);

				m_lines.push_back(buf);
				
				if(endoftext) break;

				while(num_newlines > 0)
					m_lines.push_back(""), num_newlines--;

				buf.clear();
			}
		}
	}
	else if(m_type == "CG")
    {
        const std::string path = getResourceFilename("gfx/CGLogo.bmp", gKeenFiles.gameDir, true, true);
        mpLogoBMP.reset( SDL_LoadBMP(GetFullFileName(path).c_str()), &SDL_FreeSurface );

        m_lines.push_back("Commander Genius is an interpreter");
        m_lines.push_back("made with the goal of recreating");
        m_lines.push_back("the engine that was used to power");
        m_lines.push_back("the Commander Keen series.");
        m_lines.push_back("");
        m_lines.push_back("However, we are also trying to add");
        m_lines.push_back("better support for modern systems");
        m_lines.push_back("to the games, so they can run more");
        m_lines.push_back("smoothly than they did under DOS.");
        m_lines.push_back("");
        m_lines.push_back("Thank you for supporting us by");
        m_lines.push_back("downloading Commander Genius and");
        m_lines.push_back("we hope you will report any bugs.");
    }

    switch(ExeFile.getEpisode())
    {
        case 1:
            // Change the ugly lower Tiles which are seen, when using 320x240 base resolution
            for(int i=0; i<30 ; i++)
            {
                mpMap->changeTile(22+i, 15, 14*13);
                mpMap->changeTile(22+i, 16, 14*13+3);
            }
            break;
    }

    m_logo_rect.x = m_logo_rect.y = 0;
    m_logo_rect.h = m_logo_rect.w = 0;

    if(mpLogoBMP)
    {
        m_logo_rect.w = mpLogoBMP->w;
        m_logo_rect.h = mpLogoBMP->h;
        m_logo_rect.x = 160-m_logo_rect.w/2;
        m_logo_rect.y = 22;
    }

    SDL_Surface *temp = CG_CreateRGBSurface( gVideoDriver.getGameResolution().SDLRect() );
//#if SDL_VERSION_ATLEAST(2, 0, 0)

//#else
    mpDrawSfc.reset(gVideoDriver.convertThroughBlitSfc(temp), &SDL_FreeSurface);
//#endif
    SDL_FreeSurface(temp);
}


void CAbout::ponder()
{
    if(gInput.getPressedAnyKey() || gInput.getPressedAnyCommand())
        m_destroy_me=true;
}

void CAbout::render()
{
    mpMap->animateAllTiles();
    gVideoDriver.blitScrollSurface();

    if(m_type == "ID")
    {
        mp_bmp->draw( 160-mp_bmp->getWidth()/2, 22);
    }
    else if(m_type == "CG")
    {
        if(mpLogoBMP)
            BlitSurface(mpLogoBMP.get(), nullptr, gVideoDriver.getBlitSurface(), &m_logo_rect);
    }

    for(std::size_t i=0 ; i<m_lines.size() ; i++)
    {
        gGraphics.getFont(1).drawFont(mpDrawSfc.get(), m_lines.at(i), 24, 72+i*8, true);
    }

    BlitSurface(mpDrawSfc.get(), nullptr, gVideoDriver.getBlitSurface(), nullptr);
}

void CAbout::teardown()
{
    if(!m_lines.empty())
        m_lines.clear();
    mpDrawSfc = NULL;
    mpMap = NULL;
    CEventContainer &EventContainer = gEventManager;
    EventContainer.add(new ResetScrollSurface);
}

