/*
 * CSDLVideo.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 */

#include "CSDLVideo.h"
#include "CVideoEngine.h"
#include <base/GsLogging.h>

#include "graphics/GsGraphics.h"

CSDLVideo::CSDLVideo(const CVidConfig& VidConfig) :
CVideoEngine(VidConfig)
{}

bool CSDLVideo::init()
{
    if(!CVideoEngine::init())
        return false;

#if SDL_VERSION_ATLEAST(2, 0, 0)    

    if(m_VidConfig.mRenderScQuality == "linear")
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    else
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    const int aspW = m_VidConfig.mAspectCorrection.w;
    const int aspH = m_VidConfig.mAspectCorrection.h;

    if(window)
    {
        SDL_DestroyWindow(window);
    }

    Uint32 flags = 0;

    if(m_VidConfig.Fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;
    else
        flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);


    window = SDL_CreateWindow("Commander Genius",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              m_VidConfig.m_DisplayRect.w,
                              m_VidConfig.m_DisplayRect.h,
                              flags);

    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    updateAspectRect(m_VidConfig.m_DisplayRect, aspW, aspH);

    resizeDisplayScreen(m_VidConfig.m_DisplayRect);

#else

    mDisplaySfc.setPtr(SDL_SetVideoMode( m_VidConfig.m_DisplayRect.w, m_VidConfig.m_DisplayRect.h, 32, m_Mode ));

    if (mDisplaySfc.empty())
	{
		gLogging.textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}


#endif

	return true;
}


bool CSDLVideo::resizeDisplayScreen(const GsRect<Uint16>& newDim)
{
    const int w = m_VidConfig.mAspectCorrection.w;
    const int h = m_VidConfig.mAspectCorrection.h;

    updateAspectRect(newDim, w, h);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_RenderSetLogicalSize(renderer, mAspectCorrectionRect.w, mAspectCorrectionRect.h);
#else
    mDisplaySfc.setPtr(SDL_SetVideoMode( mAspectCorrectionRect.w, mAspectCorrectionRect.h, 32, m_Mode ));
#endif

    return true;
}


bool CSDLVideo::initOverlaySurface(const Uint16 width,
                                   const Uint16 height )
{

    mOverlaySurface.create(m_Mode, width, height, RES_BPP,
                           0,0,0,0);
#if SDL_VERSION_ATLEAST(2, 0, 0)
    mOverlaySurface.setBlendMode(SDL_BLENDMODE_BLEND);
#endif
    mOverlaySurface.setAlpha(0);

    return true;
}


void CSDLVideo::setLightIntensity(const float intensity)
{
    Uint8 intense = Uint8(intensity*255.0f);

    mOverlaySurface.setAlpha(255-intense);
    mOverlaySurface.fillRGB(0, 0, 0);
}


void CSDLVideo::collectSurfaces()
{
    if( mOverlaySurface.getAlpha() > 0 )
    {
        mOverlaySurface.blitTo(mGameSfc);
    }
}

void CSDLVideo::clearSurfaces()
{
    mOverlaySurface.fillRGB(0,0,0);
    mGameSfc.fillRGB(0,0,0);
}



void CSDLVideo::transformScreenToDisplay()
{

#if SDL_VERSION_ATLEAST(2, 0, 0)            
    mpScreenSfc->lock();
    SDL_UpdateTexture(mpSdlTexture.get(), nullptr, mpScreenSfc->getSDLSurface()->pixels, mpScreenSfc->width() * sizeof (Uint32));
    mpScreenSfc->unlock();

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, mpSdlTexture.get(), NULL, NULL);
    SDL_RenderPresent(renderer);
#else

    // Blit the stuff
    mpScreenSfc->blitScaledTo(mDisplaySfc);

    // Flip the screen (We use double-buffering on some systems.)
    mDisplaySfc.flip();

#endif

}
