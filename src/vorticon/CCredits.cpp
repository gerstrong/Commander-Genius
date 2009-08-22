/*
 * CCredits.cpp
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CCredits.h"
#include "../sdl/CInput.h"
#include "../CGraphics.h"
#include "../include/menu.h"
#include "../include/gamedo.h"

CCredits::CCredits() {
}

CCredits::~CCredits() {
	// TODO Auto-generated destructor stub
}

void CCredits::Render(stCloneKeenPlus *pCKP)
{
	int mid[51];
	char scrolltext[51][80];
	unsigned char pagenumber = 0;
	int timer = 8;
	int scrolly = -51*8;
	bool cancel = false;

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;

	showmapatpos(90, 104<<4, 32, pCKP);

	memset(scrolltext,0,51*80);

	do
	{
		// do fades
		gamedo_fades();
		gamedo_AnimatedTiles();

		if(fade.mode != FADE_COMPLETE)
			continue;

		if(timer<15) timer++;
		else
		{
			timer=0;
			if(scrolly>-51*8) scrolly--;
			else
			{
				scrolly = 200;

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
					strcpy(scrolltext[25],"Handheld Devices (Wiz):");
					strcpy(scrolltext[26],"         Pickle");
					strcpy(scrolltext[27],"Resources:");
					strcpy(scrolltext[28],"         Tulip");
					strcpy(scrolltext[29],"         DaVince");
					strcpy(scrolltext[30],"");
					strcpy(scrolltext[31],"");
					strcpy(scrolltext[32],"\"As a child, I spent way too");
					strcpy(scrolltext[33],"much time playing these games.");
					strcpy(scrolltext[34],"Thanks to ID Software");
					strcpy(scrolltext[35],"for the wonderful");
					strcpy(scrolltext[36],"\"Commander Keen\" series.");
					strcpy(scrolltext[37],"");
					strcpy(scrolltext[38],"And now I have spent way");
					strcpy(scrolltext[39],"too much time programming");
					strcpy(scrolltext[40],"this game.");
					strcpy(scrolltext[41],"");
					strcpy(scrolltext[42],"...");
					strcpy(scrolltext[43],"Hmmm... Does history repeat itself?");
					strcpy(scrolltext[44],":)");
					strcpy(scrolltext[45],"");
					strcpy(scrolltext[46],"This is my tribute to");
					strcpy(scrolltext[47],"the \"Keen legacy\".");
					strcpy(scrolltext[48],"");
					strcpy(scrolltext[49],"Enjoy the Game.\"");
					strcpy(scrolltext[50],"               -Katy");
					break;

				default: cancel = true; break;
				}

				for(int j=0 ; j<51 ; j++)
					mid[j] = (320-(strlen(scrolltext[j])<<3))>>1;
				pagenumber++;
			}
		}

		for(int j=0 ; j<51 ; j++)
			if(scrolly+(j<<3) > -8 && scrolly+(j<<3) < 200)
				g_pGraphics->sb_font_draw_inverse( scrolltext[j], mid[j], scrolly+(j<<3));

		if( g_pInput->getPressedAnyCommand() )
		{
			cancel = true;
			fade.dir = FADE_OUT;
			fade.curamt = PAL_FADE_SHADES;
			fade.fadetimer = 0;
			fade.rate = FADE_NORM;
			fade.mode = FADE_GO;
		}

		if(g_pInput->getExitEvent()) cancel=true;

		g_pInput->pollEvents();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();
	} while(!(cancel && fade.mode == FADE_COMPLETE));


}
