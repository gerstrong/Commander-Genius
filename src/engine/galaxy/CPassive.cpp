/*
 * CPassive.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPassive.h"

#include "graphics/CGfxEngine.h"
#include "graphics/effects/CPixelate.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "sdl/extensions.h"


namespace galaxy
{

CPassiveGalaxy::CPassiveGalaxy() :
processMode(&CPassiveGalaxy::processIntro),
m_BackgroundBitmap(*g_pGfxEngine->getBitmap("TITLE"))
{ }

bool CPassiveGalaxy::init(char mode)
{
	// TODO: Coding has still to be done!!

	return true;
};

// This function is always called from the base class.
// Here it will execute the mode we are currently running
void CPassiveGalaxy::process()
{
	(this->*processMode)();
}

// This will show the animation of the intro you see in every galaxy game...
// Letters are big and scrolling around the screen...
void CPassiveGalaxy::processIntro()
{
	processMode = &CPassiveGalaxy::processTitle;

	m_BackgroundBitmap = *g_pGfxEngine->getBitmap("TITLE");

	SDL_FillRect(g_pVideoDriver->getBlitSurface(), NULL, 0x0);
	g_pGfxEngine->setupEffect(new CPixelate(4));

	SDL_Surface *temp = CG_CreateRGBSurface( g_pVideoDriver->getGameResolution().SDLRect() );
	mpTextSfc.reset(SDL_DisplayFormatAlpha(temp), &SDL_FreeSurface );
	SDL_FreeSurface(temp);

	createOutlinedText( 5, 170, "Press" );
	createOutlinedText( 5, 180, "Back(Esc)" );
	createOutlinedText( 5, 190, "for Menu" );

}

// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
	// draw the title bitmap here!
	m_BackgroundBitmap.draw(0, 0);

	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask(mpTextSfc, NULL, NULL) );
}


}
