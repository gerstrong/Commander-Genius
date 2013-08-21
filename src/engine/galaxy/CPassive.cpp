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
#include "core/CGameLauncherMenu.h"
#include "common/Menu/CMenuController.h"
#include "graphics/effects/CEffects.h"


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
void CPassiveGalaxy::ponder()
{		
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	
	if( EventContainer.occurredEvent<EventEndGamePlay>() )
	{
		EventContainer.pop_Event();
		m_modeg = true;
	}
}

void CPassiveGalaxy::render()
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
	g_pGfxEngine->setupEffect(new CPixelate(2));

	SDL_Surface *temp = CG_CreateRGBSurface( g_pVideoDriver->getGameResolution().SDLRect() );
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    mpTextSfc.reset(g_pVideoDriver->convertThroughBlitSfc(temp), &SDL_FreeSurface );
//#endif
	SDL_FreeSurface(temp);
}

// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
    // If something is pressed popup the menu
	if( !g_pGfxEngine->runningEffect() && !gpMenuController->active() )
	{
		if( g_pInput->getPressedAnyCommand() )
		{
		    gpMenuController->openMainMenu();
		}	    
	}
    
	// draw the title bitmap here!
	m_BackgroundBitmap.draw(0, 0);

    SDL_BlitSurface(mpTextSfc.get(), nullptr, g_pVideoDriver->getBlitSurface(), nullptr);
}


}
