/*
 * COrderingInfo.cpp
 *
 *  Created on: 04.08.2009
 *      Author: gerstrong
 */

#include "COrderingInfo.h"
#include <base/CInput.h>
#include <base/GsLogging.h>
#include <base/video/CVideoDriver.h>
#include <graphics/GsGraphics.h>

#include "fileio/CExeFile.h"
#include "CVorticonMapLoader.h"
#include "sdl/extensions.h"
#include "fileio/KeenFiles.h"


void COrderingInfo::init()
{
    CInfoScene::init();
    CExeFile &ExeFile = gKeenFiles.exeFile;
    std::string datadirectory = gKeenFiles.gameDir;
	char episode = ExeFile.getEpisode();

	mpMap.reset(new CMap);

	CVorticonMapLoaderBase Maploader(mpMap);
	
	Maploader.load(episode, 90, datadirectory);
	mpMap->gotoPos( 22<<4, 32 );
	
	// Get the offset where in the data the info is...
	size_t offset = 0;
	switch(episode)
	{
		case 1:
			m_starty = 4; // start of y-coordinate in textheights
			m_numberoflines = 21; // numberof lines to print
			if(ExeFile.getEXEVersion() == 131)
				offset = 0x1632B;

			// Change the ugly lower Tiles which are seen, when using 320x240 base resolution
			for(int i=0; i<20 ; i++)
			{
				mpMap->changeTile(22+i, 15, 14*13);
				mpMap->changeTile(22+i, 16, 14*13+3);
			}

			break;
		case 2:
			m_starty = 3; // start of y-coordinate in textheights
			m_numberoflines = 19; // numberof lines to print
			mpMap->gotoPos( 22<<4, 28 );
			if(ExeFile.getEXEVersion() == 131)
				offset = 0x1AAD9;
			break;
		case 3:
			m_starty = 4; // start of y-coordinate in textheights
			m_numberoflines = 17; // numberof lines to print
			if(ExeFile.getEXEVersion() == 131)
				offset = 0x1CBED;
			break;
	}
	mpMap->drawAll();

	// Read the strings and save them the string array of the class
	if(offset)
	{
		char *data;
		data = (char*)ExeFile.getRawData() + offset;
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
			m_Textline.push_back(buf);
			
			buf.clear();
		}
	}
	
	// This just makes them all line up exactly like in the original games.
	switch(episode)
	{
		case 1:
			m_Textline[1] = " " + m_Textline[1];
			m_Textline[2] = m_Textline[2] + "  ";
			m_Textline[3] = m_Textline[3] + " ";
			m_Textline[4] = " " + m_Textline[4];
			m_Textline[5] = " " + m_Textline[5];
			m_Textline[6] = " " + m_Textline[6];
			m_Textline[8] = m_Textline[8] + "   ";
			m_Textline[9] = m_Textline[9] + "   ";
			m_Textline[10] = m_Textline[10] + "     ";
			m_Textline[11] = m_Textline[11] + "           ";
			m_Textline[13] = m_Textline[13] + "  ";
			m_Textline[14] = m_Textline[14] + "  ";
			m_Textline[15] = m_Textline[15] + "  ";
			m_Textline[20] = m_Textline[20] + "   ";
			break;
		case 2:
			m_Textline[2] = m_Textline[2] + "     ";
			m_Textline[4] = m_Textline[4] + " ";
			m_Textline[5] = m_Textline[5] + " ";
			m_Textline[6] = m_Textline[6] + " ";
			m_Textline[7] = m_Textline[7] + "   ";
			m_Textline[8] = m_Textline[8] + "         ";
			m_Textline[10] = m_Textline[10] + "           ";
			m_Textline[11] = m_Textline[11] + "           ";
			m_Textline[12] = m_Textline[12] + "           ";
			m_Textline[13] = m_Textline[13] + "         ";
			m_Textline[15] = m_Textline[15] + "  ";
			m_Textline[16] = m_Textline[16] + "    ";
			break;
		case 3:
			m_Textline[0] = m_Textline[0] + "     ";
			m_Textline[1] = m_Textline[1] + "   ";
			m_Textline[2] = m_Textline[2] + "       ";
			m_Textline[4] = m_Textline[4] + "   ";
			m_Textline[5] = m_Textline[5] + "   ";
			m_Textline[6] = m_Textline[6] + "   ";
			m_Textline[7] = m_Textline[7] + "     ";
			m_Textline[8] = m_Textline[8] + "           ";
			m_Textline[10] = m_Textline[10] + "    ";
			m_Textline[11] = m_Textline[11] + "   ";
			m_Textline[12] = m_Textline[12] + " ";
			m_Textline[13] = m_Textline[13] + "      ";
			m_Textline[16] = m_Textline[16] + "  ";
			break;
	}

	SDL_Surface *temp = CG_CreateRGBSurface( gVideoDriver.getGameResolution().SDLRect() );
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    mpTextSfc.reset(gVideoDriver.convertThroughBlitSfc(temp), &SDL_FreeSurface);
//#endif
	SDL_FreeSurface(temp);
}

void COrderingInfo::ponder()
{	 
	if(m_Textline.empty())
	{
		gLogging.textOut(RED,"Sorry, but the ordering information text could not be read. Returning to the main menu...<br>");
		m_destroy_me=true;
		return;
	}

	if(gInput.getPressedAnyKey() || gInput.getPressedAnyCommand())
		m_destroy_me=true;
}

void COrderingInfo::render()
{
    mpMap->animateAllTiles();
    gVideoDriver.blitScrollSurface();

    for(int i=0 ; i<m_numberoflines ; i++)
    {
        gGraphics.getFont(1).drawFont(mpTextSfc.get(), m_Textline[i],
                                            160-m_Textline[i].size()*4, 8*(i+m_starty), true);
    }

    BlitSurface(mpTextSfc.get(), nullptr, gVideoDriver.getBlitSurface(), nullptr);
}

void COrderingInfo::teardown()
{
	if(!m_Textline.empty())
		m_Textline.clear();
	mpMap = NULL;
	CEventContainer &EventContainer = gEventManager;
	EventContainer.add(new ResetScrollSurface);
}
