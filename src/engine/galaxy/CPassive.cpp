/*
 * CPassive.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPassive.h"

#include "graphics/CGfxEngine.h"
#include "graphics/effects/CPixelate.h"
#include "graphics/effects/CColorMerge.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"


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

	g_pGfxEngine->setupEffect(new CPixelate(4));
}

// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
	// draw the title bitmap here!
	m_BackgroundBitmap.draw(0, 0);
}


}
