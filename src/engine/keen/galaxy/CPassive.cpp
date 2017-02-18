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
#include "menu/MainMenu.h"
#include "sdl/audio/music/CMusic.h"

#include "engine/core/VGamepads/vgamepadsimple.h"



SDL_Surface *commanderSfc;
SDL_Surface *keenSfc;


// 10 Seconds are for 1200 logic cycles
const int INTRO_TIME = 1200;
const int STARWARS_TIME = 1200;
const int STARWARS_SCROLL_TIME = 3;



namespace galaxy
{

CPassiveGalaxy::CPassiveGalaxy() :
processPonderMode(&CPassiveGalaxy::processIntro),
processRenderMode(&CPassiveGalaxy::renderIntro),
mBackgroundTitle(*gGraphics.getBitmapFromStr("TITLE")),
mBackgroundStarWars(*gGraphics.getBitmapFromStr("STARWARS")),
mCommanderTextSfc(gGraphics.getMiscGsBitmap(0)),
mKeenTextSfc(gGraphics.getMiscGsBitmap(1)),
mSkipSection(false)
{
    const GsRect<Uint16> gameRect = gVideoDriver.getVidConfig().m_GameRect;
    gVideoDriver.setNativeResolution(gameRect);

    const int episode = gpBehaviorEngine->getEpisode();

    if(episode == 4)
        mCreditsBmpID = 98;
    else if(episode == 5)
        mCreditsBmpID = 77;
    else
        mCreditsBmpID = 23;

    mCurrentLogoBmp = gGraphics.getBitmapFromId(mCreditsBmpID);

    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();

    mScaleFactor = gameRes.h/mCommanderTextSfc.height();

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

    mCommanderTextPos = Vector2D<int>(gameRes.w, (gameRes.h-cmdTextRect.h)/2 );
    mKeenTextPos = Vector2D<int>(-mKeenTextSfc.width(), (gameRes.h-cmdTextRect.h)/2 );

    GsRect<Uint16> logoBmpRect;
    logoBmpRect.w = mCurrentLogoBmp.width();
    logoBmpRect.h = mCurrentLogoBmp.height();
    logoBmpRect.x = logoBmpRect.y = 0;
    logoBmpRect.h *= mScaleFactor;
    logoBmpRect.w *= mScaleFactor;
    mCurrentLogoBmp.scaleTo(logoBmpRect);
    mCurrentLogoBmp.setColorKey( 0, 0, 0 );
    mCurrentLogoBmp.optimizeSurface();
    mCurrentLogoBmp.exchangeColor( 0x0 , 0xa8, 0x0,
                                   0x55, 0x55 , 0xFF);

    mCommanderTextSfc.setPerSurfaceAlpha(128);
    mKeenTextSfc.setPerSurfaceAlpha(128);


    mZoomSurface.create(0,
                        cmdTextRect.w+
                        keenTextRect.w+
                        mMaxSeparationWidth,
                        cmdTextRect.h,
                        32, 0, 0, 0, 0);

    gInput.flushAll();
}

bool CPassiveGalaxy::init()
{
    auto blit = gVideoDriver.getBlitSurface();
    SDL_FillRect( blit, NULL, SDL_MapRGB(blit->format,0,0,0));
    gInput.flushAll();

#ifdef TOUCHCONTROLS
    gInput.mpVirtPad.reset(new VirtualKeenControl);
    gInput.mpVirtPad->init();

    VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
    assert(vkc);
    vkc->mShowDPad = false;

#endif

    const auto &storyText = gpBehaviorEngine->getString("STORY_TEXT");
    mStoryTextVector = explode(storyText, "\n");

    return true;
}

// This function is always called from the base class.
// Here it will execute the mode we are currently running
void CPassiveGalaxy::ponder(const float deltaT)
{		
    if(gEffectController.runningEffect())
       return;

    if( gInput.getPressedAnyCommand() || gInput.mouseClicked() )
    {
        mSkipSection = true;
        g_pMusicPlayer->stop();
    }

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

    const int logoPosX = (gameRes.w-mCurrentLogoBmp.width())/2;

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

    const int logoMidPosY = mLogoPosY+mCurrentLogoBmp.height()/2;

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
        if(mLogoPosY+mCurrentLogoBmp.height() <= 0)
        {
            mLogoPosY = gameRes.h;
            mTerminatorLogoNum++;
            mTerminatorTimer = 0;

            mCurrentLogoBmp = gGraphics.getBitmapFromId(mCreditsBmpID+mTerminatorLogoNum);
            mCurrentLogoBmp.optimizeSurface();

            GsRect<Uint16> logoBmpRect;
            logoBmpRect.w = mCurrentLogoBmp.width();
            logoBmpRect.h = mCurrentLogoBmp.height();
            logoBmpRect.x = logoBmpRect.y = 0;
            logoBmpRect.h *= mScaleFactor;
            logoBmpRect.w *= mScaleFactor;

            mCurrentLogoBmp.scaleTo(logoBmpRect);            
            mCurrentLogoBmp.setColorKey( 0, 0, 0 );
            mCurrentLogoBmp.optimizeSurface();
            mCurrentLogoBmp.exchangeColor( 0x00, 0xa8, 0x00,
                                           0x55, 0x55, 0xFF);
        }
    }

    const int textSeparation = (mCommanderTextPos.x+mCommanderTextSfc.width()) - mKeenTextPos.x;

    if(textSeparation <= -mMaxSeparationWidth || mSkipSection)
    {        
        mZoomSfcPos.x = (gameRes.w-mZoomSurface.width())/2;
        mZoomSfcZoom.x = mZoomSurface.width();
        mZoomSfcZoom.y = mZoomSurface.height();
        processPonderMode = &CPassiveGalaxy::processIntroZoom;
        processRenderMode = &CPassiveGalaxy::renderIntroZoom;

        gInput.flushAll();

        mCommanderTextSfc._draw(0,0, mZoomSurface.getSDLSurface() );
        mKeenTextSfc._draw(mCommanderTextSfc.width()+59*mScaleFactor, 0, mZoomSurface.getSDLSurface() );

        mTerminatorTimer = 0;
        mSkipSection = false;
    }
}

void CPassiveGalaxy::processIntroZoom()
{
    const int leftEdge = 8;
    const int topEdge = 8;
    const int maxWidth = (19*gVideoDriver.getGameResolution().w)/20;

    if(mZoomSfcPos.x < leftEdge)
    {
        mZoomSfcPos.x += 20;
        mZoomSfcZoom.x -= 3;
    }
    else
    {
        mZoomSfcPos.x = leftEdge;
    }

    if(mZoomSfcZoom.x > maxWidth)
    {
        mZoomSfcZoom.x -= mScaleFactor*36;
    }
    else
    {
        mZoomSfcZoom.x = maxWidth;
    }


    if(mZoomSfcPos.y > topEdge)
    {
        mZoomSfcPos.y -= 5;
        mZoomSfcZoom.y -= 2;
    }
    else
    {
        mZoomSfcPos.y = topEdge;
    }

    if(mZoomSfcZoom.y > mScaleFactor*32)
    {
        mZoomSfcZoom.y -= mScaleFactor*4;
    }



    if( (mZoomSfcPos.x >= leftEdge &&
         mZoomSfcPos.y <= topEdge &&
         mZoomSfcZoom.x <= maxWidth &&
         mZoomSfcZoom.y <= mScaleFactor*32 ) ||
         mSkipSection)
    {
        gInput.flushAll();
        processPonderMode = &CPassiveGalaxy::processTitle;
        processRenderMode = &CPassiveGalaxy::renderTitle;
        mBackgroundTitle = *gGraphics.getBitmapFromStr("TITLE");

        mIntroTimer = INTRO_TIME;

        GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
        mBackgroundTitle.scaleTo(gameRes);
        renderIntroZoom();
        gEffectController.setupEffect(new CPixelate(2));
        mSkipSection = false;
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

    SDL_Surface *zoomSfc = mZoomSurface.getSDLSurface();

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

    blitScaled( zoomSfc, srGsRect, blitSfc, dstRect, vidConf.m_ScaleXFilter );
}


// Just show the title screen with the pixelation effect
void CPassiveGalaxy::processTitle()
{
    // If something is pressed at this section, open the menu
    if( !gEffectController.runningEffect() && !gMenuController.active() )
	{
        if( mSkipSection )
		{
            gInput.flushAll();

#ifdef TOUCHCONTROLS
            VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
            assert(vkc);
            vkc->mShowDPad = true;
#endif

            gEventManager.add(new OpenMainMenuEvent());
            mSkipSection = false;
		}	    
    }

    mIntroTimer--;

    if(mIntroTimer <= 0)
    {
        mIntroTimer = 0;

        processPonderMode = &CPassiveGalaxy::processStarWars;
        processRenderMode = &CPassiveGalaxy::renderStarWars;
        mBackgroundStarWars = *gGraphics.getBitmapFromStr("STARWARS");

        GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
        mBackgroundStarWars.scaleTo(gameRes);


        GsSurface darkener;

        darkener.create(0, gameRes.w, gameRes.h, 32,
                        0, 0, 0, 0);

        darkener.setBlendMode(SDL_BLENDMODE_BLEND);
        darkener.setAlpha(128);

        auto *sfc = mBackgroundStarWars.getSDLSurface();

        SDL_BlitSurface(darkener.getSDLSurface(), nullptr, sfc, nullptr);

        //mBackgroundStarWars


        SDL_Rect lRect;


        // Draw the title of the story text
        GsFont &starwarsFont = gGraphics.getFont(2);

        SDL_Rect gameResSDL = gameRes.SDLRect();

        const auto reqHeight = mStoryTextVector.size()*20;

        mStarwarsTextSfc.create(0, 320, reqHeight, 32,
                                0, 0, 0, 0);

        // Black becomes transparent
        mStarwarsTextSfc.setColorKey(0, 0, 0);

        // Scroll position of the star wars text.
        mStarwarsScrollPos = -gameResSDL.h;
        mStarwarsTimer = 0;


        lRect.h = mStarwarsTextSfc.height();    lRect.w = mStarwarsTextSfc.width();
        lRect.x = 0;                            lRect.y = 0;

        for(int i=0 ; i<mStoryTextVector.size() ; i++)
        {
            starwarsFont.drawFontCentered( mStarwarsTextSfc.getSDLSurface(), mStoryTextVector[i], lRect.x, lRect.w, lRect.y, false);
            lRect.y += 20;
        }

        const auto ep = gpBehaviorEngine->getEpisode();

        if(ep == 4)
        {
            g_pMusicPlayer->loadTrack(2);
            g_pMusicPlayer->play();
        }
        else if(ep == 5)
        {
            g_pMusicPlayer->loadTrack(2);
            g_pMusicPlayer->play();
        }
        else if(ep == 6)
        {
            g_pMusicPlayer->loadTrack(4);
            g_pMusicPlayer->play();
        }

    }
}

void CPassiveGalaxy::processStarWars()
{
    // If something is pressed at this section, open the menu
    if( !gEffectController.runningEffect() && !gMenuController.active() )
    {
        if( mSkipSection )
        {
            gInput.flushAll();

#ifdef TOUCHCONTROLS
            VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
            assert(vkc);
            vkc->mShowDPad = true;
#endif

            gEventManager.add(new OpenMainMenuEvent());
            mSkipSection = false;
        }
    }

    // TODO: need to put K1ngDuke code here in C++ translated. (maybe...)

    if(mStarwarsTimer > STARWARS_SCROLL_TIME)
    {
        mStarwarsTimer = 0;
        mStarwarsScrollPos++;
    }
    else
    {
        mStarwarsTimer++;
    }

    auto *starwarsSfc = mStarwarsTextSfc.getSDLSurface();

    SDL_Rect starwarsRect = starwarsSfc->clip_rect;

    if(mStarwarsScrollPos >= starwarsRect.h)
    {
        processPonderMode = &CPassiveGalaxy::processIntro;
        processRenderMode = &CPassiveGalaxy::renderIntro;

        g_pMusicPlayer->stop();
    }
}



void CPassiveGalaxy::renderTitle()
{
    // draw the title bitmap here!
    mBackgroundTitle.draw(0, 0);
}

void CPassiveGalaxy::renderStarWars()
{
    // draw the title bitmap here!
    mBackgroundStarWars.draw(0, 0);

    // TODO: Need to render the transformed text here!
    GsWeakSurface sfc(gVideoDriver.getBlitSurface());

    SDL_Rect lRect;

    lRect.h = sfc.height();    lRect.w = sfc.width();
    lRect.x = 0;        lRect.y = 20;



    GsRect<Uint16> gameRes = gVideoDriver.getGameResolution();
    SDL_Rect gameResSDL = gameRes.SDLRect();

    SDL_Surface *blitSfc = gVideoDriver.getBlitSurface();

    CVidConfig &vidConf = gVideoDriver.getVidConfig();


    auto *starwarsSfc = mStarwarsTextSfc.getSDLSurface();

    SDL_Rect starwarsRect = starwarsSfc->clip_rect;

    if(mStarwarsScrollPos < starwarsRect.h)
    {
        SDL_Rect dstRect = gameResSDL;

        starwarsRect.h = dstRect.h;

        // Blit the scrolling surface
        if(mStarwarsScrollPos < 0)
        {
            dstRect.y = -mStarwarsScrollPos;
        }
        else
        {
            starwarsRect.y = mStarwarsScrollPos;
        }

        blitScaled( starwarsSfc, starwarsRect, blitSfc, dstRect, NONE );
    }

    lRect.h = sfc.height();    lRect.w = sfc.width();
    lRect.x = 0;        lRect.y = lRect.h-20;

    // Draw some text.
    GsFont &Font = gGraphics.getFont(1);

    if(mSwapColors)
    {
        Font.setupColor(0xFFFFFF);
    }
    else
    {
        Font.setupColor(0xFF0000);
    }

    /*if(mStarWarsTimer % 60 == 0)
    {
        mSwapColors = !mSwapColors;
    }


    Font.drawFontCentered( sfc.getSDLSurface(), "Press any to start...", lRect.x, lRect.w, lRect.y, false);

    mStarWarsTimer--;

    if(mStarWarsTimer <= 0)
    {
        mStarWarsTimer = STARWARS_TIME;
    }*/
}

}
