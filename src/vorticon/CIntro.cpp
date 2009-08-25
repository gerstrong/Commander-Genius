/*
 * CIntro.cpp
 *
 *  Created on: 19.06.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CIntro.h"
#include "../sdl/CInput.h"
#include "../CGraphics.h"
#include "../include/menu.h"
#include "../include/gamedo.h"
#include "../common/palette.h"

CIntro::CIntro() {
}

CIntro::~CIntro() {
}

void CIntro::Render(stCloneKeenPlus *pCKP)
{
	// TODO: Now that the new intro function works, the old one must become
	// a credits class

	int bmnum[7];
	int mid[7];
	int timer = 0;
	int introtime = 5000;  // Total time to elapse until Main menu opens
	int scrolly = 200;
	bool cancel=false;

	fade( FADE_IN, FADE_NORM );
	showmapatpos(90, 104<<4, 32, pCKP);

	// Load the Title Bitmap
	bmnum[0] = g_pGraphics->getBitmapNumberFromName("AN");
	bmnum[1] = g_pGraphics->getBitmapNumberFromName("APOGEE");
	bmnum[2] = g_pGraphics->getBitmapNumberFromName("PRESENT");
	bmnum[3] = g_pGraphics->getBitmapNumberFromName("OFAN");
	bmnum[4] = g_pGraphics->getBitmapNumberFromName("IDSOFT");
	bmnum[5] = g_pGraphics->getBitmapNumberFromName("SOFTWARE");
	bmnum[6] = g_pGraphics->getBitmapNumberFromName("PRODUCT");

	for(int j=0 ; j<7 ; j++)
		mid[j] = (320/2)-(bitmaps[bmnum[j]].xsize/2);

	g_pInput->flushAll();

	do
	{
		// do fades
		gamedo_fades();
		g_pGraphics->drawBitmap2FG(mid[1], scrolly+9, bmnum[1]);

		if(fade_in_progress())
			continue;

		if(timer<7) timer++;
		else
		{
			timer=0;
			if(scrolly>35)	scrolly--;
		}

		g_pGraphics->drawBitmap2FG(mid[1], scrolly+9, bmnum[1]);

		if(scrolly<=35) // Show this, when scrolling is finished
		{
			g_pGraphics->drawBitmap2FG(mid[0], scrolly, bmnum[0]);
			g_pGraphics->drawBitmap2FG(mid[2], scrolly+43, bmnum[2]);
			g_pGraphics->drawBitmap2FG(mid[3], scrolly+56, bmnum[3]);
			g_pGraphics->drawBitmap2FG(mid[4], scrolly+77, bmnum[4]);
			g_pGraphics->drawBitmap2FG(mid[5], scrolly+87, bmnum[5]);
			g_pGraphics->drawBitmap2FG(mid[6], scrolly+120, bmnum[6]);
		}

		gamedo_AnimatedTiles();

		if( g_pInput->getPressedAnyKey() || g_pInput->getPressedAnyCommand() )
		{
			cancel = true;
			fade(FADE_OUT, FADE_NORM);
		}

		if(g_pInput->getExitEvent()) cancel=true;

		g_pInput->pollEvents();

		if( introtime <= 0 ) break;
		introtime--;
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();

	} while(!(cancel /*&& fade.mode == FADE_COMPLETE*/));


}
