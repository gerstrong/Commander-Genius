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

    const int scaleFactor = gameRes.h/mCommanderTextSfc.getHeight();

    mMaxSeparationWidth = 60*scaleFactor;

    CRect<Uint16> cmdTextRect, keenTextRect;
    cmdTextRect.w = mCommanderTextSfc.getSDLSurface()->w;
    cmdTextRect.x = cmdTextRect.y = 0;
    cmdTextRect.h = mCommanderTextSfc.getSDLSurface()->h;
    keenTextRect.w = mKeenTextSfc.getSDLSurface()->w;
    keenTextRect.h = mKeenTextSfc.getSDLSurface()->h;
    keenTextRect.x = keenTextRect.y = 0;

    cmdTextRect.h *= scaleFactor;
    cmdTextRect.w *= scaleFactor;
    keenTextRect.h *= scaleFactor;
    keenTextRect.w *= scaleFactor;

    mCommanderTextSfc.scaleTo(cmdTextRect);
    mKeenTextSfc.scaleTo(keenTextRect);

    mCommanderTextPos = VectorD2<int>(gameRes.w, 0);
    mKeenTextPos = VectorD2<int>(-mKeenTextSfc.getWidth(), 0);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod( mCommanderTextSfc.getSDLSurface(), 128);
    SDL_SetSurfaceAlphaMod( mKeenTextSfc.getSDLSurface(), 128);
#else
    SDL_SetAlpha(mCommanderTextSfc.getSDLSurface(), SDL_SRCALPHA, 128);
    SDL_SetAlpha(mKeenTextSfc.getSDLSurface(), SDL_SRCALPHA, 128);
#endif
}

bool CPassiveGalaxy::init(char mode)
{
    auto blit = g_pVideoDriver->getBlitSurface();
    SDL_FillRect( blit, NULL, SDL_MapRGB(blit->format,0,0,0));

	return true;
}

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


// TODO: This will show the animation of the intro you see in every galaxy game...
// Letters are big and scrolling around the screen...
void CPassiveGalaxy::processIntro()
{	       
    mCommanderTextSfc.draw(mCommanderTextPos.x, mCommanderTextPos.y);
    mCommanderTextPos.x -= 2;

    mKeenTextSfc.draw(mKeenTextPos.x, mKeenTextPos.y);
    mKeenTextPos.x++;

    mTimer++;

    const int textSeparation = (mCommanderTextPos.x+mCommanderTextSfc.getWidth()) - mKeenTextPos.x;

    if(textSeparation <= -mMaxSeparationWidth || g_pInput->getPressedAnyCommand())
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
