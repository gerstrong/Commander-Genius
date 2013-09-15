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
    const int episode = g_pBehaviorEngine->getEpisode();

    if(episode == 4)
        mCreditsBmpID = 98;
    else if(episode == 5)
        mCreditsBmpID = 77;
    else
        mCreditsBmpID = 23;

    mCurrentLogoBmp = g_pGfxEngine->getBitmap(mCreditsBmpID);

    CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();

    mScaleFactor = gameRes.h/mCommanderTextSfc.getHeight();

    mTerminatorTimer = 0;
    mTerminatorLogoNum = 0;
    mLogoPosY = gameRes.h;

    mMaxSeparationWidth = 60*mScaleFactor;

    // Scale Bitmaps to adapt the resolutions
    CRect<Uint16> cmdTextRect, keenTextRect, logoBmp;
    cmdTextRect.w = mCommanderTextSfc.getSDLSurface()->w;
    cmdTextRect.x = cmdTextRect.y = 0;
    cmdTextRect.h = mCommanderTextSfc.getSDLSurface()->h;
    keenTextRect.w = mKeenTextSfc.getSDLSurface()->w;
    keenTextRect.h = mKeenTextSfc.getSDLSurface()->h;
    keenTextRect.x = keenTextRect.y = 0;

    cmdTextRect.h *= mScaleFactor;
    cmdTextRect.w *= mScaleFactor;
    keenTextRect.h *= mScaleFactor;
    keenTextRect.w *= mScaleFactor;

    mCommanderTextSfc.scaleTo(cmdTextRect);
    mKeenTextSfc.scaleTo(keenTextRect);

    mCommanderTextPos = VectorD2<int>(gameRes.w, (gameRes.h-cmdTextRect.h)/2 );
    mKeenTextPos = VectorD2<int>(-mKeenTextSfc.getWidth(), (gameRes.h-cmdTextRect.h)/2 );

    CRect<Uint16> logoBmpRect;
    logoBmpRect.w = mCurrentLogoBmp.getWidth();
    logoBmpRect.h = mCurrentLogoBmp.getHeight();
    logoBmpRect.x = logoBmpRect.y = 0;
    logoBmpRect.h *= mScaleFactor;
    logoBmpRect.w *= mScaleFactor;
    mCurrentLogoBmp.scaleTo(logoBmpRect);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod( mCommanderTextSfc.getSDLSurface(), 128);
    SDL_SetSurfaceAlphaMod( mKeenTextSfc.getSDLSurface(), 128);
#else
    SDL_SetAlpha(mCommanderTextSfc.getSDLSurface(), SDL_SRCALPHA, 128);
    SDL_SetAlpha(mKeenTextSfc.getSDLSurface(), SDL_SRCALPHA, 128);
#endif

    mpZoomSurface.reset( SDL_CreateRGBSurface(0,
                                            cmdTextRect.w+
                                            keenTextRect.w+
                                            mMaxSeparationWidth,
                                            cmdTextRect.h,
                                            32, 0, 0, 0, 0), &SDL_FreeSurface );

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


const int logoStayTime = 150;
const int logoSpeed = 1;

// TODO: This will show the animation of the intro you see in every galaxy game...
// Letters are big and scrolling around the screen...
void CPassiveGalaxy::processIntro()
{	       
    CBitmap &bmpLogo = g_pGfxEngine->getBitmap(mTerminatorLogoNum+mCreditsBmpID);
    CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
    SDL_Rect gameResSDL = gameRes.SDLRect();

    const int logoPosX = (gameRes.w-bmpLogo.getWidth())/2;
    const int logoMidPosY = mLogoPosY+bmpLogo.getHeight()/2;

    SDL_Surface *blitSfc = g_pVideoDriver->getBlitSurface();
    SDL_FillRect( blitSfc, &gameResSDL, SDL_MapRGB(blitSfc->format, 0, 0, 0) );


    if(mTerminatorLogoNum < 4)
    {
        // Step Logo comes in
        // Logo stays between first step and Third step time
        // Logo goes away
        if(logoMidPosY > gameRes.h/2 || mTerminatorTimer > logoStayTime)
            mLogoPosY -= (logoSpeed*mScaleFactor);
        else
            mTerminatorTimer++;

        bmpLogo.draw(logoPosX, mLogoPosY);

        // Change Logo
        if(mLogoPosY+bmpLogo.getHeight() <= 0)
        {
            mLogoPosY = gameRes.h;
            mTerminatorLogoNum++;
            mTerminatorTimer = 0;

            mCurrentLogoBmp = g_pGfxEngine->getBitmap(mCreditsBmpID+mTerminatorLogoNum);
            CRect<Uint16> logoBmpRect;
            logoBmpRect.w = mCurrentLogoBmp.getWidth();
            logoBmpRect.h = mCurrentLogoBmp.getHeight();
            logoBmpRect.x = logoBmpRect.y = 0;
            logoBmpRect.h *= mScaleFactor;
            logoBmpRect.w *= mScaleFactor;
            mCurrentLogoBmp.scaleTo(logoBmpRect);
        }
    }

    mCommanderTextSfc.draw(mCommanderTextPos.x, mCommanderTextPos.y);
    mCommanderTextPos.x -= 2;

    mKeenTextSfc.draw(mKeenTextPos.x, mKeenTextPos.y);
    mKeenTextPos.x++;

    const int textSeparation = (mCommanderTextPos.x+mCommanderTextSfc.getWidth()) - mKeenTextPos.x;
    if(textSeparation <= -mMaxSeparationWidth || g_pInput->getPressedAnyCommand())
    {        
        mZoomSfcPos.x = (gameRes.w-mpZoomSurface->w)/2;
        mZoomSfcZoom.x = mpZoomSurface->w;
        mZoomSfcZoom.y = mpZoomSurface->h;
        processMode = &CPassiveGalaxy::processIntroZoom;
        g_pInput->flushAll();

        mCommanderTextSfc._draw(0,0, mpZoomSurface.get() );
        mKeenTextSfc._draw(mCommanderTextSfc.getWidth()+59*mScaleFactor, 0, mpZoomSurface.get() );

        mTerminatorTimer = 0;
    }

}

void CPassiveGalaxy::processIntroZoom()
{
    CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
    SDL_Rect gameResSDL = gameRes.SDLRect();

    SDL_Surface *blitSfc = g_pVideoDriver->getBlitSurface();
    SDL_FillRect( blitSfc, &gameResSDL, SDL_MapRGB(blitSfc->format, 0, 0, 0) );

    SDL_Surface *zoomSfc = mpZoomSurface.get();
    SDL_Surface *blit = g_pVideoDriver->getBlitSurface();
    SDL_Rect dstRect, srcRect;

    srcRect.x = srcRect.y = 0;

    if(mZoomSfcPos.x < 16)
    {
        srcRect.x -= mZoomSfcPos.x;

        mZoomSfcPos.x += 20;
        mZoomSfcZoom.x -= 4;
    }
    else
    {
        mZoomSfcPos.x = 16;
    }

    if(mZoomSfcZoom.x > gameRes.w)
    {
       mZoomSfcZoom.x -= mScaleFactor*2*4;
    }
    else
    {
        mZoomSfcZoom.x = gameRes.w;
    }


    if(mZoomSfcPos.y > 8)
    {
        srcRect.y -= mZoomSfcPos.y;

        mZoomSfcPos.y -= 20;
        mZoomSfcZoom.y -= 4;
    }
    else
    {
        mZoomSfcPos.y = 8;
    }

    if(mZoomSfcZoom.y > mScaleFactor*32)
    {
       mZoomSfcZoom.y -= mScaleFactor*2;
    }


    // Here we define the Rects to zoom
    srcRect.w = zoomSfc->w;
    srcRect.h = zoomSfc->h;


    dstRect.x = mZoomSfcPos.x;
    dstRect.y = mZoomSfcPos.y;
    dstRect.w = mZoomSfcZoom.x;
    dstRect.h = mZoomSfcZoom.y;


#if SDL_VERSION_ATLEAST(2, 0, 0)

    int error = SDL_BlitScaled( zoomSfc, &srcRect, blit, &dstRect );
    if(error)
    {
        std::string errorMsg = SDL_GetError();
        g_pLogFile->textOut("SDL_Blit failed: " + errorMsg);
        return;
    }
#else
            g_pLogFile->textOut("SDL 1.2 doesn't support Bitmap scaling to higher resolutions, "
                                "please let the devs know about this problem.");
    return false;
#endif

    if( (mZoomSfcPos.x >= 16 &&
         mZoomSfcPos.y <= 8 &&
         mZoomSfcZoom.x <= gameRes.w &&
         mZoomSfcZoom.y <= mScaleFactor*32 ) ||
         g_pInput->getPressedAnyCommand())
    {
        processMode = &CPassiveGalaxy::processTitle;
        m_BackgroundBitmap = *g_pGfxEngine->getBitmap("TITLE");

        CRect<Uint16> gameRes = g_pVideoDriver->getGameResolution();
        m_BackgroundBitmap.scaleTo(gameRes);
        g_pGfxEngine->setupEffect(new CPixelate(2));
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
}


}
