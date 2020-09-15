/*
 * CCredits.cpp
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#include "CCredits.h"
#include <base/CInput.h>
#include <base/GsApp.h>
#include <base/video/CVideoDriver.h>
#include "graphics/GsGraphics.h"
#include "CVorticonMapLoader.h"


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
	
    m_scrolltext[0] = gApp.getName();
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
    m_scrolltext[16] = "Gagster";
	m_scrolltext[17] = "";
	m_scrolltext[18] = "Handheld Devices:";
	m_scrolltext[19] = "Pelya";
	m_scrolltext[20] = "Albert Zeyer";
	m_scrolltext[21] = "Elias Oenal";
	m_scrolltext[22] = "";
	m_scrolltext[23] = "Resources:";
	m_scrolltext[24] = "DaVince";
	m_scrolltext[25] = "";
	m_scrolltext[26] = "";
	m_scrolltext[27] = "Commander Keen";
	m_scrolltext[28] = "Developed by";
	m_scrolltext[29] = "ID Software";
	m_scrolltext[30] = "";
	m_scrolltext[31] = "Tom Hall";
	m_scrolltext[32] = "";
	m_scrolltext[33] = "John D. Carmack";
	m_scrolltext[34] = "";
	m_scrolltext[35] = "John Romero";
	m_scrolltext[36] = "";
	m_scrolltext[37] = "Adrian Carmack";
	m_scrolltext[38] = "";
	m_scrolltext[39] = "Published by";
	m_scrolltext[40] = "Apogee";
	m_scrolltext[41] = "";
	m_scrolltext[42] = "Scott Miller";
	m_scrolltext[43] = "";
	m_scrolltext[44] = "Music by";
	m_scrolltext[45] = "Bobby Prince";
	m_scrolltext[46] = "";
	m_scrolltext[47] = "Special Thanks to";
	m_scrolltext[48] = "Katy, for making the";
	m_scrolltext[49] = "CloneKeen Engine";
	m_scrolltext[50] = "in the first place.";
	m_scrolltext[51] = "";
	m_scrolltext[52] = "The Commander Keen";
	m_scrolltext[53] = "Community for all of";
	m_scrolltext[54] = "the support they give.";
	
	m_timer = 0;
    m_scrolly = -m_scrolltext.size()*8;

    for(size_t j=0 ; j<m_scrolltext.size() ; j++)
    {
		m_mid[j] = 160-(m_scrolltext[j].size()*4);
    }

    mDrawSfc.createRGBSurface( gVideoDriver.getGameResolution().SDLRect() );
    mDrawSfc.makeBlitCompatible();
}

void CCredits::ponder()
{
	if(m_timer<2) m_timer++;
	else
	{
		m_timer=0;
        if(m_scrolly>-(int(m_scrolltext.size()))*8)
        {
            m_scrolly--;
        }
		else
        {
            m_scrolly = gVideoDriver.getGameResolution().dim.y;
        }
	}
	

	if( gInput.getPressedAnyKey() || gInput.getPressedAnyCommand() )
		m_destroy_me = true;
}

void CCredits::render()
{
    auto &creditFont = gGraphics.getFontLegacy(0);
    mpMap->animateAllTiles();
    gVideoDriver.blitScrollSurfaces();

    mDrawSfc.fillRGB(0, 0, 0);

    for(size_t j=0 ; j<m_scrolltext.size() ; j++)
    {
        const int boundary = m_scrolly+(j<<3);

        if(boundary > -8 &&
           boundary < gVideoDriver.getGameResolution().dim.y)
        {
            creditFont.drawFont( mDrawSfc, m_scrolltext[j],
                                 m_mid[j], m_scrolly+(j<<3), true);
        }
    }

    GsWeakSurface weakBlit(gVideoDriver.getBlitSurface());
    mDrawSfc.blitScaledTo(weakBlit);
}


void CCredits::teardown()
{
    mpMap = nullptr;
	CEventContainer &EventContainer = gEventManager;
	EventContainer.add(new ResetScrollSurface);
}
