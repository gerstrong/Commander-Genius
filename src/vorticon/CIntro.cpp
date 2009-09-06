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
#include "../sdl/CVideoDriver.h"

CIntro::CIntro() {
}

CIntro::~CIntro() {
}

void CIntro::Render(stCloneKeenPlus *pCKP)
{
	CBitmap* bm[6];
	int mid[6];
	int timer = 0;
	int introtime = 5000;  // Total time to elapse until Main menu opens
	int scrolly = 200;
	bool cancel=false;

	showmapatpos(90, 104<<4, 32, pCKP);

	// Load the Title Bitmap
	bm[0] = g_pGfxEngine->getBitmap("AN");
	bm[1] = g_pGfxEngine->getBitmap("APOGEE");
	bm[2] = g_pGfxEngine->getBitmap("PRESENT");
	bm[3] = g_pGfxEngine->getBitmap("OFAN");
	bm[4] = g_pGfxEngine->getBitmap("IDSOFT");
	bm[5] = g_pGfxEngine->getBitmap("PRODUCT");

	for(int j=0 ; j<6 ; j++)
		mid[j] = (320/2)-(bm[j]->getWidth()/2);

	g_pInput->flushAll();

	do
	{
		bm[1]->draw( g_pVideoDriver->FGLayerSurface, mid[1], scrolly+9);

		if(timer<7) timer++;
		else
		{
			timer=0;
			if(scrolly>35)	scrolly--;
		}

		bm[1]->draw( g_pVideoDriver->FGLayerSurface, mid[1], scrolly+9);

		if(scrolly<=35) // Show this, when scrolling is finished
		{
			bm[0]->draw( g_pVideoDriver->FGLayerSurface, mid[0], scrolly);
			bm[2]->draw( g_pVideoDriver->FGLayerSurface, mid[2], scrolly+43);
			bm[3]->draw( g_pVideoDriver->FGLayerSurface, mid[3], scrolly+56);
			bm[4]->draw( g_pVideoDriver->FGLayerSurface, mid[4], scrolly+77);
			bm[5]->draw( g_pVideoDriver->FGLayerSurface, mid[5], scrolly+120);
		}

		gamedo_AnimatedTiles();

		if( g_pInput->getPressedAnyKey() || g_pInput->getPressedAnyCommand() )
			cancel = true;

		if(g_pInput->getExitEvent()) cancel=true;

		g_pInput->pollEvents();

		if( introtime <= 0 ) break;
		introtime--;
		// blit the scrollbuffer to the display
		gamedo_frameskipping_blitonly();

	} while(!cancel);


}
