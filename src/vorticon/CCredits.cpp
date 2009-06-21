/*
 * CCredits.cpp
 *
 *  Created on: 20.06.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CCredits.h"
#include "../sdl/CTimer.h"
#include "../sdl/CInput.h"
#include "../CGraphics.h"
#include "../include/menu.h"
#include "../include/gamedo.h"

CCredits::CCredits() {
	// TODO Auto-generated constructor stub

}

CCredits::~CCredits() {
	// TODO Auto-generated destructor stub
}

void CCredits::Render(stCloneKeenPlus *pCKP)
{
	// TODO: Now that the new intro function works, the old one must become
	// a credits class

	int mid[7];
	char scrolltext[7][80];
	unsigned char pagenumber = 0;
	int timer = 8;
	int scrolly = 0;
	bool cancel = false;

	fade.mode = FADE_GO;
	fade.rate = FADE_NORM;
	fade.dir = FADE_IN;
	fade.curamt = 0;
	fade.fadetimer = 0;

	showmapatpos(90, 104<<4, 32, 0, pCKP);

	memset(scrolltext,0,7*80);

	do
	{
		// do fades
		gamedo_fades();

		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();

		gamedo_AnimatedTiles();

		if(fade.mode != FADE_COMPLETE)
			continue;

		if(timer<15) timer++;
		else
		{
			timer=0;
			if(scrolly>-7*8) scrolly--;
			else
			{
				scrolly = 200;
				memset(scrolltext,0,7*80);

				switch(pagenumber)
				{
				case 0:
					strcpy(scrolltext[0],"Commander Genius");
					strcpy(scrolltext[1],"Interpreter of");
					strcpy(scrolltext[2],"Commander Keen 1-3");
					strcpy(scrolltext[3],"Credits");
					break;
				case 1:
					strcpy(scrolltext[0],"based on the engine of");
					strcpy(scrolltext[1],"CloneKeen by Shaw");
					strcpy(scrolltext[2],"and");
					strcpy(scrolltext[3],"CloneKeenPlus by Gerstrong");
					break;

				case 2:
					strcpy(scrolltext[0],"Developers");
					strcpy(scrolltext[1],"of");
					strcpy(scrolltext[2],"Commander Genius");
					strcpy(scrolltext[3],"");
					strcpy(scrolltext[4],"Aka CloneKeenPlus");
					break;

				case 3:
					strcpy(scrolltext[0],"Main Developer:");
					strcpy(scrolltext[1],"         Gerstrong");
					strcpy(scrolltext[2],"Handheld Devices (Wiz):");
					strcpy(scrolltext[3],"         Pickle");
					strcpy(scrolltext[4],"Resources:");
					strcpy(scrolltext[5],"         Tulip");
					break;

				case 4:
					strcpy(scrolltext[0],"Thanks to ID Software");
					strcpy(scrolltext[1],"for the wonderful");
					strcpy(scrolltext[2],"\"Commander Keen\" series.");
					strcpy(scrolltext[3],"");
					strcpy(scrolltext[4],"\"As a child, I spent way too");
					strcpy(scrolltext[5],"much time playing these games.");
					break;


				case 5:
					strcpy(scrolltext[0],"And now I have spent way");
					strcpy(scrolltext[1],"too much time programming");
					strcpy(scrolltext[2],"this game.");
					strcpy(scrolltext[3],"");
					strcpy(scrolltext[4],"...");
					strcpy(scrolltext[5],"Hmmm... Does history repeat itself?");
					strcpy(scrolltext[6],":)");
					break;

				case 6:
					strcpy(scrolltext[0],"This is my tribute to");
					strcpy(scrolltext[1],"the \"Keen legacy\".");
					strcpy(scrolltext[2],"");
					strcpy(scrolltext[3],"Enjoy the Game.\"");
					strcpy(scrolltext[4],"");
					strcpy(scrolltext[5],"               -Katy");
					break;

				default: cancel = true; break;
				}

				for(int j=0 ; j<7 ; j++)
					mid[j] = (320-(strlen(scrolltext[j])<<3))>>1;
				pagenumber++;
			}
		}

		for(int j=0 ; j<7 ; j++)
			g_pGraphics->sb_font_draw_inverse( (unsigned char*) scrolltext[j], mid[j], scrolly+(j<<3));

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
	    g_pTimer->SpeedThrottle();

	} while(!(cancel && fade.mode == FADE_COMPLETE));


}
