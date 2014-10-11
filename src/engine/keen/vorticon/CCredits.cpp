/*
 * CCredits.cpp
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#include "CCredits.h"
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>
#include "graphics/GsGraphics.h"
#include "CVorticonMapLoader.h"
#include "sdl/extensions.h"

#include <fileio/KeenFiles.h>

void CCredits::init()
{
    CInfoScene::init();
	mpMap.reset( new CMap );
	
	//creditFont = gGraphics.getFont(0);
	//creditFont.tintColor(SDL_MapRGB( creditFont.getSDLSurface()->format, 255, 0, 0) );

	CVorticonMapLoaderBase Maploader(mpMap);

    CExeFile &ExeFile = gKeenFiles.exeFile;

    Maploader.load( ExeFile.getEpisode(), 90, gKeenFiles.gameDir );
	mpMap->gotoPos( 104<<4, 16 );
	
	m_scrolltext[0] = "Commander Genius";
	m_scrolltext[1] = "aka CloneKeenPlus";
	m_scrolltext[2] = "";
	m_scrolltext[3] = "";
	m_scrolltext[4] = "based on the engine of";
	m_scrolltext[5] = "CloneKeen by Shaw";
	m_scrolltext[6] = "";
	m_scrolltext[7] = "";
	m_scrolltext[8] = "";
	m_scrolltext[9] = "Developers:";
	m_scrolltext[10] = "";
	m_scrolltext[11] = "Main Developers:";
	m_scrolltext[12] = "Gerhard Stein (Gerstrong)";
	m_scrolltext[13] = "Albert Zeyer";
	m_scrolltext[14] = "Chad Ian Anderson (Pizza2004)";
	m_scrolltext[15] = "Martin Hauber (Tulip)";
	m_scrolltext[16] = "Scott Smith (Pickle)";
	m_scrolltext[17] = "";
	m_scrolltext[18] = "Handheld Devices:";
	m_scrolltext[19] = "Pelya";
	m_scrolltext[20] = "Albert Zeyer";
	m_scrolltext[21] = "";
	m_scrolltext[22] = "Resources:";
	m_scrolltext[23] = "DaVince";
	m_scrolltext[24] = "";
	m_scrolltext[25] = "";
	m_scrolltext[26] = "Commander Keen";
	m_scrolltext[27] = "Developed by";
	m_scrolltext[28] = "ID Software";
	m_scrolltext[29] = "";
	m_scrolltext[30] = "Tom Hall";
	m_scrolltext[31] = "";
	m_scrolltext[32] = "John D. Carmack";
	m_scrolltext[33] = "";
	m_scrolltext[34] = "John Romero";
	m_scrolltext[35] = "";
	m_scrolltext[36] = "Adrian Carmack";
	m_scrolltext[37] = "";
	m_scrolltext[38] = "Published by";
	m_scrolltext[39] = "Apogee";
	m_scrolltext[40] = "";
	m_scrolltext[41] = "Scott Miller";
	m_scrolltext[42] = "";
	m_scrolltext[43] = "Music by";
	m_scrolltext[44] = "Bobby Prince";
	m_scrolltext[45] = "";
	m_scrolltext[46] = "Special Thanks to";
	m_scrolltext[47] = "Katy, for making the";
	m_scrolltext[48] = "CloneKeen Engine";
	m_scrolltext[49] = "in the first place.";
	m_scrolltext[50] = "";
	m_scrolltext[51] = "The Commander Keen";
	m_scrolltext[52] = "Community for all of";
	m_scrolltext[53] = "the support they give.";
	
	m_timer = 0;
	m_scrolly = -54*8;

	for(int j=0 ; j<54 ; j++)
		m_mid[j] = 160-(m_scrolltext[j].size()*4);

	SDL_Surface *temp = CG_CreateRGBSurface( gVideoDriver.getGameResolution().SDLRect() );
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    mpDrawSfc.reset(gVideoDriver.convertThroughBlitSfc(temp), &SDL_FreeSurface);
//#endif
	SDL_FreeSurface(temp);
}

void CCredits::ponder()
{
	if(m_timer<2) m_timer++;
	else
	{
		m_timer=0;
		if(m_scrolly>-54*8) m_scrolly--;
		else
			m_scrolly = gVideoDriver.getGameResolution().h;
	}
	

	if( gInput.getPressedAnyKey() || gInput.getPressedAnyCommand() )
		m_destroy_me = true;
}

void CCredits::render()
{
    GsFont &creditFont = gGraphics.getFont(0);
    mpMap->animateAllTiles();
    gVideoDriver.blitScrollSurface();

    SDL_FillRect(mpDrawSfc.get(), NULL, 0x0);

    for(int j=0 ; j<54 ; j++)
    {
        if(m_scrolly+(j<<3) > -8 && m_scrolly+(j<<3) < gVideoDriver.getGameResolution().h)
        {
            creditFont.drawFont( mpDrawSfc.get(), m_scrolltext[j], m_mid[j], m_scrolly+(j<<3), true);
        }
    }

    BlitSurface(mpDrawSfc.get(), nullptr, gVideoDriver.getBlitSurface(), nullptr);
}


void CCredits::teardown()
{
	mpDrawSfc = NULL;
	mpMap = NULL;
	CEventContainer &EventContainer = gEventManager;
	EventContainer.add(new ResetScrollSurface);
}
