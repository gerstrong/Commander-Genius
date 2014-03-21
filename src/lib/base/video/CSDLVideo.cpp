/*
 * CSDLVideo.cpp
 *
 *  Created on: 05.02.2011
 *      Author: gerstrong
 */

#include "CSDLVideo.h"
#include "CVideoEngine.h"
#include <lib/base/GsLogging.h>

#include "graphics/GsGraphics.h"
#include "graphics/PerSurfaceAlpha.h"

CSDLVideo::CSDLVideo(const CVidConfig& VidConfig) :
CVideoEngine(VidConfig)
{}

bool CSDLVideo::init()
{
    if(!CVideoEngine::init())
        return false;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    // NOTE: try not to free the last SDL_Surface of the screen, this is freed automatically by SDL
    const int aspW = m_VidConfig.mAspectCorrection.w;
    const int aspH = m_VidConfig.mAspectCorrection.h;

    if(window)
    {
        SDL_DestroyWindow(window);
    }

    window = SDL_CreateWindow("Commander Genius",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              m_VidConfig.m_DisplayRect.w,
                              m_VidConfig.m_DisplayRect.h,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    updateAspectRect(m_VidConfig.m_DisplayRect, aspW, aspH);

    //SDL_RenderSetLogicalSize(renderer, m_VidConfig.m_DisplayRect.w, m_VidConfig.m_DisplayRect.h);
    resizeDisplayScreen(m_VidConfig.m_DisplayRect);

#else

    mDisplaySfc = SDL_SetVideoMode( m_VidConfig.m_DisplayRect.w, m_VidConfig.m_DisplayRect.h, 32, m_Mode );

    if (!mDisplaySfc)
	{
		gLogging.textOut(RED,"VidDrv_Start(): Couldn't create a SDL surface: %s<br>", SDL_GetError());
		return false;
	}

    const GsRect<Uint16> &GameRect = m_VidConfig.m_GameRect;
    m_src_slice = GameRect.w*mDisplaySfc->format->BytesPerPixel;

    /*aspectCorrectResizing(newDim, w, h);

    if(FilteredSurface)
    {
        Scaler.setDynamicFactor( float(FilteredSurface->w)/float(aspectCorrectionRect.w),
                                 float(FilteredSurface->h)/float(aspectCorrectionRect.h));
    }*/

#endif

	return true;
}


bool CSDLVideo::resizeDisplayScreen(const GsRect<Uint16>& newDim)
{
    //const int w = m_VidConfig.mAspectCorrection.w;
    //const int h = m_VidConfig.mAspectCorrection.h;
    const int w = 0;
    const int h = 0;

    updateAspectRect(newDim, w, h);

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_RenderSetLogicalSize(renderer, mAspectCorrectionRect.w, mAspectCorrectionRect.h);
#endif

    return true;
}


bool CSDLVideo::initOverlaySurface( const bool useAlpha,
                                       const Uint16 width,
                                       const Uint16 height )
{

    SDL_Surface *overlay = createSurface( "OverlaySurface",
                                         useAlpha,
                                         width,
                                         height,
                                         RES_BPP,
                                         m_Mode );

    mpOverlaySurface.reset( overlay );

    if(!mpOverlaySurface)
        return false;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceBlendMode( overlay, SDL_BLENDMODE_BLEND);
    SDL_SetSurfaceAlphaMod( overlay, 0);
#else
    SDL_SetAlpha( overlay, SDL_SRCALPHA, 0);
#endif


    return true;
}


void CSDLVideo::setLightIntensity(const float intensity)
{
    Uint8 intense = Uint8(intensity*255.0f);

    auto *sfc = mpOverlaySurface.get();
    Uint32 color = SDL_MapRGB(sfc->format, 0, 0, 0);

    #if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod( sfc, 255-intense);
    #else
        SDL_SetAlpha( sfc, SDL_SRCALPHA, 255-intense);
    #endif

    SDL_FillRect( sfc, nullptr, color);
}


void CSDLVideo::collectSurfaces()
{
    SDL_Surface *overlay = mpOverlaySurface.get();

    if( getPerSurfaceAlpha(overlay) )
        SDL_BlitSurface(overlay, NULL, mpGameSfc.get(), NULL);
}

void CSDLVideo::clearSurfaces()
{
    SDL_FillRect(mpOverlaySurface.get(), NULL, 0x0);
    SDL_FillRect(mpGameSfc.get(), NULL, 0x0);
}



void CSDLVideo::transformScreenToDisplay()
{
    /*
	if( Scaler.filterFactor() <= 1 &&
			BlitSurface->h == aspectCorrectionRect.h &&
			BlitSurface->w == aspectCorrectionRect.w )
	{
		SDL_Rect scrrect, dstrect;
		scrrect.y = 0;
		scrrect.x = 0;
		dstrect.y = aspectCorrectionRect.y;
		dstrect.x = aspectCorrectionRect.x;
		dstrect.h = scrrect.h = BlitSurface->h;
		dstrect.w = scrrect.w = BlitSurface->w;

        #if SDL_VERSION_ATLEAST(2, 0, 0)
            SDL_LockSurface(BlitSurface);
            SDL_UpdateTexture(sdlTexture, nullptr, BlitSurface->pixels, BlitSurface->w * sizeof (Uint32));
            SDL_UnlockSurface(BlitSurface);
        #else
            SDL_BlitSurface(BlitSurface, &scrrect, screen, &dstrect);
        #endif
	}
	else
	{
		// First apply the conventional filter if any (GameScreen -> FilteredScreen)
        Scaler.scaleUp(FilteredSurface, BlitSurface, SCALEX, aspectCorrectionRect);

        #if SDL_VERSION_ATLEAST(2, 0, 0)
            SDL_LockSurface(FilteredSurface);
            SDL_UpdateTexture(sdlTexture, nullptr, FilteredSurface->pixels, FilteredSurface->w * sizeof (Uint32));
            SDL_UnlockSurface(FilteredSurface);
        #else
            // Now scale up to the new DisplayRect (FilteredScreen -> screen)
            Scaler.scaleUp(screen, FilteredSurface, DYNAMIC, aspectCorrectionRect);
        #endif
    }*/


#if SDL_VERSION_ATLEAST(2, 0, 0)
    auto screen = mpScreenSfc.get();
    SDL_LockSurface(screen);
    SDL_UpdateTexture(mpSdlTexture.get(), nullptr, screen->pixels, screen->w * sizeof (Uint32));
    SDL_UnlockSurface(screen);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, mpSdlTexture.get(), NULL, NULL);
    SDL_RenderPresent(renderer);
#else

    SDL_BlitSurface(mpScreenSfc.get(), NULL, mDisplaySfc, NULL);

	// Flip the screen (We use double-buffering on some systems.)
    SDL_Flip(mDisplaySfc);
#endif

}
