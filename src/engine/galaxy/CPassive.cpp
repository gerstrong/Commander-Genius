/*
 * CPassive.cpp
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#include <base/video/CVideoDriver.h>
#include <base/CInput.h>
#include <widgets/GsMenuController.h>
#include <base/video/GsEffectController.h>


#include "CPassive.h"
#include "graphics/GsGraphics.h"
#include "graphics/effects/CPixelate.h"
#include "sdl/extensions.h"
#include "common/Menu/CMainMenu.h"


namespace galaxy
{

CPassiveGalaxy::CPassiveGalaxy() :
processPonderMode(&CPassiveGalaxy::processIntro),
processRenderMode(&CPassiveGalaxy::renderIntro),
m_BackgroundBitmap(*gGraphics.getBitmapFromStr("TITLE")),
mCommanderTextSfc(gGraphics.getMisGsBitmap(0)),
mKeenTextSfc(gGraphics.getMisGsBitmap(1))
{
    const int episode = g_pBehaviorEngine->getEpisode();

    if(episode == 4)
        mCreditsBmpID = 98;
    else if(episode == 5)
        mCreditsBmpID = 77;
    else
        mCreditsBmpID = 23;

    mCurrentLogoBmp = gGraphics.getBitmapFromId(mCreditsBmpID);

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    mScaleFactor = gameRes.h/mCommanderTextSfc.getHeight();

    mTerminatorTimer = 0;
    mTerminatorLogoNum = 0;
    mLogoPosY = gameRes.h;

    mMaxSeparationWidth = 60*mScaleFactor;

    // Scale Bitmaps to adapt the resolutions
    GsRect<Uint16> cmdTextRect, keenTextRect;
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
    mCommanderTextSfc.setColorKey( 0, 0, 0 );
    mKeenTextSfc.scaleTo(keenTextRect);
    mKeenTextSfc.setColorKey( 0, 0, 0 );

    mCommanderTextPos = VectorD2<int>(gameRes.w, (gameRes.h-cmdTextRect.h)/2 );
    mKeenTextPos = VectorD2<int>(-mKeenTextSfc.getWidth(), (gameRes.h-cmdTextRect.h)/2 );

    GsRect<Uint16> logoBmpRect;
    logoBmpRect.w = mCurrentLogoBmp.getWidth();
    logoBmpRect.h = mCurrentLogoBmp.getHeight();
    logoBmpRect.x = logoBmpRect.y = 0;
    logoBmpRect.h *= mScaleFactor;
    logoBmpRect.w *= mScaleFactor;
    mCurrentLogoBmp.scaleTo(logoBmpRect);
    mCurrentLogoBmp.setColorKey( 0, 0, 0 );
    mCurrentLogoBmp.optimizeSurface();
    mCurrentLogoBmp.exchangeColor( 0x0 , 0xa8, 0x0,
                                   0x55, 0x55 , 0xFF);


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
    gInput.flushAll();
}

bool CPassiveGalaxy::init(char mode)
{
    auto blit = gVideoDriver.getBlitSurface();
    SDL_FillRect( blit, NULL, SDL_MapRGB(blit->format,0,0,0));

	return true;
}

// This function is always called from the base class.
// Here it will execute the mode we are currently running
void CPassiveGalaxy::ponder()
{		
    (this->*processPonderMode)();
}

void CPassiveGalaxy::render()
{
    (this->*processRenderMode)();
}


const int logoStayTime = 150;
const int logoSpeed = 1;

void CPassiveGalaxy::renderIntro()
{
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    SDL_Rect gameResSDL = gameRes.SDLRect();

    const int logoPosX = (gameRes.w-mCurrentLogoBmp.getWidth())/2;

    SDL_Surface *blitSfc = gVideoDriver.getBlitSurface();
    SDL_FillRect( blitSfc, &gameResSDL, SDL_MapRGB(blitSfc->format, 0, 0, 0) );

    mCommanderTextSfc.draw(mCommanderTextPos.x, mCommanderTextPos.y);

    mKeenTextSfc.draw(mKeenTextPos.x, mKeenTextPos.y);
    if(mTerminatorLogoNum < 4)
    {
        mCurrentLogoBmp.draw(logoPosX, mLogoPosY);
    }

}


// TODO: This will show the animation of the intro you see in every galaxy game...
// Letters are big and scrolling around the screen...
void CPassiveGalaxy::processIntro()
{	       
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    const int logoMidPosY = mLogoPosY+mCurrentLogoBmp.getHeight()/2;

    mCommanderTextPos.x -= 2;
    mKeenTextPos.x++;

    if(mTerminatorLogoNum < 4)
    {
        // Step Logo comes in
        // Logo stays between first step and Third step time
        // Logo goes away
        if(logoMidPosY > gameRes.h/2 || mTerminatorTimer > logoStayTime)
            mLogoPosY -= (logoSpeed*mScaleFactor);
        else
            mTerminatorTimer++;

        // Change Logo
        if(mLogoPosY+mCurrentLogoBmp.getHeight() <= 0)
        {
            mLogoPosY = gameRes.h;
            mTerminatorLogoNum++;
            mTerminatorTimer = 0;

            mCurrentLogoBmp = gGraphics.getBitmapFromId(mCreditsBmpID+mTerminatorLogoNum);
            mCurrentLogoBmp.optimizeSurface();

            GsRect<Uint16> logoBmpRect;
            logoBmpRect.w = mCurrentLogoBmp.getWidth();
            logoBmpRect.h = mCurrentLogoBmp.getHeight();
            logoBmpRect.x = logoBmpRect.y = 0;
            logoBmpRect.h *= mScaleFactor;
            logoBmpRect.w *= mScaleFactor;

            mCurrentLogoBmp.scaleTo(logoBmpRect);            
            mCurrentLogoBmp.exchangeColor( 0x0 , 0xa8, 0x0,
                                           0x55, 0x55 , 0xFF);
            mCurrentLogoBmp.setColorKey( 0, 0, 0 );

        }
    }

    const int textSeparation = (mCommanderTextPos.x+mCommanderTextSfc.getWidth()) - mKeenTextPos.x;

    if(textSeparation <= -mMaxSeparationWidth || gInput.getPressedAnyCommand())
    {        
        mZoomSfcPos.x = (gameRes.w-mpZoomSurface->w)/2;
        mZoomSfcZoom.x = mpZoomSurface->w;
        mZoomSfcZoom.y = mpZoomSurface->h;
        processPonderMode = &CPassiveGalaxy::processIntroZoom;
        processRenderMode = &CPassiveGalaxy::renderIntroZoom;
        //processPonderMode = &CPassiveGalaxy::processTitle;
        //processRenderMode = &CPassiveGalaxy::renderTitle;

        gInput.flushAll();

        mCommanderTextSfc._draw(0,0, mpZoomSurface.get() );
        mKeenTextSfc._draw(mCommanderTextSfc.getWidth()+59*mScaleFactor, 0, mpZoomSurface.get() );

        mTerminatorTimer = 0;
    }
}

void CPassiveGalaxy::processIntroZoom()
{
    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    if(mZoomSfcPos.x < 16)
    {
        mZoomSfcPos.x += 20;
        mZoomSfcZoom.x -= 4;
    }
    else
    {
        mZoomSfcPos.x = 16;
    }

    if(mZoomSfcZoom.x > gameRes.w)
    {
       mZoomSfcZoom.x -= mScaleFactor*8;
    }
    else
    {
        mZoomSfcZoom.x = gameRes.w;
    }


    if(mZoomSfcPos.y > 8)
    {
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



    if( (mZoomSfcPos.x >= 16 &&
         mZoomSfcPos.y <= 8 &&
         mZoomSfcZoom.x <= gameRes.w &&
         mZoomSfcZoom.y <= mScaleFactor*32 ) ||
         gInput.getPressedAnyCommand())
    {
        gInput.flushAll();
        processPonderMode = &CPassiveGalaxy::processTitle;
        processRenderMode = &CPassiveGalaxy::renderTitle;
        m_BackgroundBitmap = *gGraphics.getBitmapFromStr("TITLE");

        GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
        m_BackgroundBitmap.scaleTo(gameRes);
        renderIntroZoom();
        gEffectController.setupEffect(new CPixelate(2));
    }
}

void CPassiveGalaxy::renderIntroZoom()
{
    SDL_Rect dstRect, srGsRect;
    srGsRect.x = srGsRect.y = 0;

    if(mZoomSfcPos.x < 16)
    {
        srGsRect.x -= mZoomSfcPos.x;
    }

    if(mZoomSfcPos.y > 8)
    {
        srGsRect.y -= mZoomSfcPos.y;
    }

    SDL_Surface *zoomSfc = mpZoomSurface.get();

    // Here we define the Rects to zoom
    srGsRect.w = zoomSfc->w;
    srGsRect.h = zoomSfc->h;


    dstRect.x = mZoomSfcPos.x;
    dstRect.y = mZoomSfcPos.y;
    dstRect.w = mZoomSfcZoom.x;
    dstRect.h = mZoomSfcZoom.y;

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    SDL_Rect gameResSDL = gameRes.SDLRect();

    SDL_Surface *blitSfc = gVideoDriver.getBlitSurface();
    SDL_FillRect( blitSfc, &gameResSDL, SDL_MapRGB(blitSfc->format, 0, 0, 0) );

    CVidConfig &vidConf = gVideoDriver.getVidConfig();

    blitScaled( zoomSfc, srGsRect, blitSfc, dstRect, vidConf.m_ScaleXFilter ); // TODO: This still makes CG crash!!
}


// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
    // If something is pressed, popup the menu
    if( !gEffectController.runningEffect() && !gMenuController.active() )
	{
		if( gInput.getPressedAnyCommand() )
		{
            gInput.flushAll();
            gEventManager.add(new OpenMainMenuEvent);
		}	    
	}    
}



void CPassiveGalaxy::renderTitle()
{
    // draw the title bitmap here!
    m_BackgroundBitmap.draw(0, 0);
}



}
