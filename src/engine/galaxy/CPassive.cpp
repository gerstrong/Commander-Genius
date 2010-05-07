/*
 * CPassive.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPassive.h"

#include "../../graphics/CGfxEngine.h"
#include "../../graphics/effects/CPixelate.h"
#include "../../graphics/effects/CColorMerge.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"

#include "GraphicsIDs.h"

namespace galaxy
{

CPassiveGalaxy::CPassiveGalaxy(char Episode, std::string DataDirectory,
		CSavedGame &SavedGame, stOption *p_Option) :
CPassive(Episode, DataDirectory, SavedGame, p_Option),
processMode(&CPassiveGalaxy::processIntro),
mp_Menu(new CMenuGalaxy(MAIN, DataDirectory, Episode, SavedGame, p_Option))
{ }

bool CPassiveGalaxy::init(char mode)
{
	return true;
}

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

	m_BackgroundBitmap = g_pGfxEngine->getBitmap(BMP_TITLE);

	g_pGfxEngine->pushEffectPtr(new CPixelate(2));
}

// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
	// draw the title bitmap here!
	m_BackgroundBitmap.draw(g_pVideoDriver->BlitSurface, 0, 0);
	
	if ( g_pInput->getPressedAnyKey() )
	{
		if(g_pGfxEngine->applyingEffects())
			g_pGfxEngine->killEffect();
		else
		{
			// Now we process the Galaxy Menu
			processMode = &CPassiveGalaxy::processMenu;
			m_BackgroundBitmap = g_pGfxEngine->getBitmap(BMP_KEENSWATCH);
			g_pGfxEngine->pushEffectPtr(new CColorMerge(8));
		}
	}
}

// This will call the menu in Passive mode and let the player choose a game
// load one, changes options, etc.
void CPassiveGalaxy::processMenu()
{
	// draw the background!
	m_BackgroundBitmap.draw(g_pVideoDriver->BlitSurface, 0, 0);

	mp_Menu->process();

	if ( g_pInput->getPressedAnyKey() )
	{
		if(g_pGfxEngine->applyingEffects())
			g_pGfxEngine->killEffect();
		else
		{
			g_pGfxEngine->pushEffectPtr(new CColorMerge(8));

			// If we press any key we go to the menu screen
			m_modeg = true;
		}
	}
}

// The cleanup function just make sure the resources are freed, when a mode is switched or the game closed
void CPassiveGalaxy::cleanup()
{

}

CPassiveGalaxy::~CPassiveGalaxy()
{
	// TODO Auto-generated destructor stub
}

}
