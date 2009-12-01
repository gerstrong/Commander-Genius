/*
 * CCredits.cpp
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#include "../../keen.h"
#include "CCredits.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../graphics/CGfxEngine.h"
#include "../../include/gamedo.h"
#include "../../common/CMapLoader.h"

CCredits::CCredits(std::string &datadirectory, char &episode) {
	mp_Scrollsurface = g_pVideoDriver->ScrollSurface;
	mp_Map = new CMap(mp_Scrollsurface, g_pGfxEngine->Tilemap);
	CMapLoader Maploader(mp_Map);
	
	Maploader.load(episode, 90, datadirectory);
	mp_Map->gotoPos( 104<<4, 32 );
	
	strcpy(m_scrolltext[0],"Commander Genius");
	strcpy(m_scrolltext[1],"aka CloneKeenPlus");
	strcpy(m_scrolltext[2],"");
	strcpy(m_scrolltext[3],"");
	strcpy(m_scrolltext[4],"based on the engine of");
	strcpy(m_scrolltext[5],"CloneKeen by Shaw");
	strcpy(m_scrolltext[6],"");
	strcpy(m_scrolltext[7],"");
	strcpy(m_scrolltext[8],"");
	strcpy(m_scrolltext[9],"Developers:");
	strcpy(m_scrolltext[10],"");
	strcpy(m_scrolltext[11],"Main Developers:");
	strcpy(m_scrolltext[12],"         Gerstrong");
	strcpy(m_scrolltext[13],"         Albert Zeyer");
	strcpy(m_scrolltext[14],"         Pizza2004");
	strcpy(m_scrolltext[15],"         Pickle");
	strcpy(m_scrolltext[16],"");
	strcpy(m_scrolltext[17],"Handheld Devices:");
	strcpy(m_scrolltext[18],"         Pickle");
	strcpy(m_scrolltext[19],"         Albert Zeyer");
	strcpy(m_scrolltext[20],"");
	strcpy(m_scrolltext[21],"Resources:");
	strcpy(m_scrolltext[22],"         Tulip");
	strcpy(m_scrolltext[23],"         DaVince");
	strcpy(m_scrolltext[24],"");
	strcpy(m_scrolltext[25],"");
	strcpy(m_scrolltext[26],"Commander Keen");
	strcpy(m_scrolltext[27],"Developed by");
	strcpy(m_scrolltext[28],"ID Software");
	strcpy(m_scrolltext[29],"");
	strcpy(m_scrolltext[30],"Tom Hall");
	strcpy(m_scrolltext[31],"");
	strcpy(m_scrolltext[32],"John D. Carmack");
	strcpy(m_scrolltext[33],"");
	strcpy(m_scrolltext[34],"John Romero");
	strcpy(m_scrolltext[35],"");
	strcpy(m_scrolltext[36],"Adrian Carmack");
	strcpy(m_scrolltext[37],"");
	strcpy(m_scrolltext[38],"Published by");
	strcpy(m_scrolltext[39],"Apogee");
	strcpy(m_scrolltext[40],"");
	strcpy(m_scrolltext[41],"Scott Miller");
	strcpy(m_scrolltext[42],"");
	strcpy(m_scrolltext[43],"Music by");
	strcpy(m_scrolltext[44],"Bobby Prince");
	strcpy(m_scrolltext[45],"");
	strcpy(m_scrolltext[46],"Special Thanks to");
	strcpy(m_scrolltext[47],"Katy, for making the");
	strcpy(m_scrolltext[48],"CloneKeen Engine");
	strcpy(m_scrolltext[49],"in the first place.");
	strcpy(m_scrolltext[50],"");
	strcpy(m_scrolltext[51],"The Commander Keen");
	strcpy(m_scrolltext[52],"Community for all of");
	strcpy(m_scrolltext[53],"the support they give.");
	
	m_timer = 0;
	m_scrolly = -54*8;
}

CCredits::~CCredits() {
	
}

void CCredits::process()
{
	int mid[54];
	
	memset(m_scrolltext,0,53*80);
	
	// do fades
	mp_Map->animateAllTiles();
	
	if(m_timer<15) m_timer++;
	else
	{
		m_timer=0;
		if(m_scrolly>-54*8) m_scrolly--;
		else
		{
			m_scrolly = g_pVideoDriver->getGameResolution().h;
			
			
			for(int j=0 ; j<54 ; j++)
				mid[j] = (320-(strlen(m_scrolltext[j])<<3))>>1;
		}
	}
	
	for(int j=0 ; j<54 ; j++)
		if(m_scrolly+(j<<3) > -8 && m_scrolly+(j<<3) < g_pVideoDriver->getGameResolution().h)
			g_pGfxEngine->Font->drawFont( g_pVideoDriver->FGLayerSurface, m_scrolltext[j], mid[j], m_scrolly+(j<<3), LETTER_TYPE_INVERSE);
	
	if( g_pInput->getPressedAnyKey() )
		m_destroy_me = true;
}
