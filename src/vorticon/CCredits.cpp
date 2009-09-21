/*
 * CCredits.cpp
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CCredits.h"
#include "../sdl/CInput.h"
#include "../sdl/CVideoDriver.h"
#include "../graphics/CGfxEngine.h"
#include "../include/CStartScreen.h"
#include "../include/gamedo.h"

CCredits::CCredits() {
}

CCredits::~CCredits() {
	// TODO Auto-generated destructor stub
}

void CCredits::Render(stCloneKeenPlus *pCKP)
{
	int mid[52];
	char scrolltext[52][80];
	unsigned char pagenumber = 0;
	int timer = 8;
	int scrolly = -52*8;
	bool cancel = false;

	showmapatpos(90, 104<<4, 32, pCKP);
	g_pInput->flushAll();
	memset(scrolltext,0,51*80);

	do
	{
		// do fades
		gamedo_AnimatedTiles();

		if(timer<15) timer++;
		else
		{
			timer=0;
			if(scrolly>-51*8) scrolly--;
			else
			{
				scrolly = g_pVideoDriver->getGameResRect().w;
				switch(pagenumber)
				{
				case 0:
					strcpy(scrolltext[0],"Commander Genius");
					strcpy(scrolltext[1],"Interpreter of");
					strcpy(scrolltext[2],"Commander Keen 1-3");
					strcpy(scrolltext[3],"Credits");
					strcpy(scrolltext[4],"");
					strcpy(scrolltext[5],"");
					strcpy(scrolltext[6],"");
					strcpy(scrolltext[7],"");
					strcpy(scrolltext[8],"based on the engine of");
					strcpy(scrolltext[9],"CloneKeen by Shaw");
					strcpy(scrolltext[10],"and");
					strcpy(scrolltext[11],"CloneKeenPlus by Gerstrong");
					strcpy(scrolltext[12],"");
					strcpy(scrolltext[13],"");
					strcpy(scrolltext[14],"");
					strcpy(scrolltext[15],"");
					strcpy(scrolltext[16],"Developers");
					strcpy(scrolltext[17],"of");
					strcpy(scrolltext[18],"Commander Genius");
					strcpy(scrolltext[19],"");
					strcpy(scrolltext[20],"Aka CloneKeenPlus");
					strcpy(scrolltext[21],"");
					strcpy(scrolltext[22],"Main Developers:");
					strcpy(scrolltext[23],"         Gerstrong");
					strcpy(scrolltext[24],"         Albert Zeyer");
					strcpy(scrolltext[25],"         Pizza2004");
					strcpy(scrolltext[26],"Handheld Devices (Wiz):");
					strcpy(scrolltext[27],"         Pickle");
					strcpy(scrolltext[28],"Resources:");
					strcpy(scrolltext[29],"         Tulip");
					strcpy(scrolltext[30],"         DaVince");
					strcpy(scrolltext[31],"");
					strcpy(scrolltext[32],"");
					strcpy(scrolltext[33],"\"As a child, I spent way too");
					strcpy(scrolltext[34],"much time playing these games.");
					strcpy(scrolltext[35],"Thanks to ID Software");
					strcpy(scrolltext[36],"for the wonderful");
					strcpy(scrolltext[37],"\"Commander Keen\" series.");
					strcpy(scrolltext[38],"");
					strcpy(scrolltext[39],"And now I have spent way");
					strcpy(scrolltext[40],"too much time programming");
					strcpy(scrolltext[41],"this game.");
					strcpy(scrolltext[42],"");
					strcpy(scrolltext[43],"...");
					strcpy(scrolltext[44],"Hmmm... Does history repeat itself?");
					strcpy(scrolltext[45],":)");
					strcpy(scrolltext[46],"");
					strcpy(scrolltext[47],"This is my tribute to");
					strcpy(scrolltext[48],"the \"Keen legacy\".");
					strcpy(scrolltext[49],"");
					strcpy(scrolltext[50],"Enjoy the Game.\"");
					strcpy(scrolltext[51],"               -Katy");
					break;

				default: cancel = true; break;
				}

				for(int j=0 ; j<52 ; j++)
					mid[j] = (320-(strlen(scrolltext[j])<<3))>>1;
				pagenumber++;
			}
		}

		for(int j=0 ; j<52 ; j++)
			if(scrolly+(j<<3) > -8 && scrolly+(j<<3) < g_pVideoDriver->getGameResRect().w)
				g_pGfxEngine->Font->drawFont( g_pVideoDriver->FGLayerSurface, scrolltext[j], mid[j], scrolly+(j<<3), LETTER_TYPE_INVERSE);

		if( g_pInput->getPressedAnyCommand() )
			cancel = true;

		if(g_pInput->getExitEvent()) cancel=true;

		g_pInput->pollEvents();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();
	} while( !cancel );

}
