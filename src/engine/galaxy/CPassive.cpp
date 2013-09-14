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
m_BackgroundBitmap(*g_pGfxEngine->getBitmap("TITLE")),
mCommanderTextSfc(g_pGfxEngine->getMiscBitmap(0)),
mKeenTextSfc(g_pGfxEngine->getMiscBitmap(1))
{
    CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
    mCommanderTextPos = VectorD2<int>(gameRes.w, 0);
    mKeenTextPos = VectorD2<int>(-mKeenTextSfc.getWidth(), gameRes.h-mKeenTextSfc.getHeight());
}

bool CPassiveGalaxy::init(char mode)
{
    auto blit = g_pVideoDriver->getBlitSurface();
    SDL_FillRect( blit, NULL, SDL_MapRGB(blit->format,0,0,0));

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

const int timeForTerminatorText = 2000;

// TODO: This will show the animation of the intro you see in every galaxy game...
// Letters are big and scrolling around the screen...
void CPassiveGalaxy::processIntro()
{	       
    mCommanderTextSfc.draw(mCommanderTextPos.x, mCommanderTextPos.y);
    mCommanderTextPos.x--;

    mKeenTextSfc.draw(mKeenTextPos.x, mKeenTextPos.y);
    mKeenTextPos.x++;

    mTimer++;

    if(mTimer == timeForTerminatorText)
    {
        processMode = &CPassiveGalaxy::processTitle;
        m_BackgroundBitmap = *g_pGfxEngine->getBitmap("TITLE");

        CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
        m_BackgroundBitmap.scaleTo(gameRes);
        g_pGfxEngine->setupEffect(new CPixelate(2));
        mTimer = 0;
    }
}

// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
    // If something is pressed, popup the menu
	if( !g_pGfxEngine->runningEffect() && !gpMenuController->active() )
	{
		if( g_pInput->getPressedAnyCommand() )
		{
		    gpMenuController->openMainMenu();
		}	    
	}
    
	// draw the title bitmap here!
	m_BackgroundBitmap.draw(0, 0);

    //SDL_BlitSurface(mpTextSfc.get(), nullptr, g_pVideoDriver->getBlitSurface(), nullptr);
}


}
