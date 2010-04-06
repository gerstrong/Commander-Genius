/*
 * CPassive.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include "CPassive.h"

#include "../../graphics/CGfxEngine.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CInput.h"

#include "GraphicsIDs.h"

namespace galaxy
{

CPassiveGalaxy::CPassiveGalaxy(char Episode, std::string DataDirectory,
		CSavedGame &SavedGame, stOption *p_Option) :
CPassive(Episode, DataDirectory, SavedGame, p_Option),
processMode(&CPassiveGalaxy::processIntro)
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
}

// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
	CBitmap &Bitmap = g_pGfxEngine->getBitmap(BMP_TITLE);

	// TODO: Add the pixelation effect here!

	// draw Bitmap here!
	Bitmap.draw(g_pVideoDriver->BlitSurface, 0, 0);
	
	if (g_pInput->getPressedAnyKey())
	{
		// If we press any key we go to the menu screen
		m_modeg = true;
	}
}

// This will call the menu in Passive mode and let the player choose a game
// load one, changes options, etc.
void CPassiveGalaxy::processMenu()
{

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
