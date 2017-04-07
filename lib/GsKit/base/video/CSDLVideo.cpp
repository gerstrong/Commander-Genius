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
    if( !CVideoEngine::init() )
    {
        return false;
    }

#if SDL_VERSION_ATLEAST(2, 0, 0)    

    if(m_VidConfig.mRenderScQuality == "linear")
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    }
    else
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    }

    const int aspW = m_VidConfig.mAspectCorrection.w;
    const int aspH = m_VidConfig.mAspectCorrection.h;

    if(window)
    {
        SDL_DestroyWindow(window);
    }

    Uint32 flags = 0;

    if(m_VidConfig.Fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    else
    {
        flags |= (SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    }


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

    Uint32 rendererFlags = 0;

    if(m_VidConfig.vsync)
    {
        rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
    }

    renderer = SDL_CreateRenderer(window, -1, rendererFlags);

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
        // TODO: If horiz borders enabled we need to reposition the cursor
        mOverlaySurface.blitTo(mGameSfc);
    }
}

void CSDLVideo::clearSurfaces()
{
    // TODO: Clear color here!
    mOverlaySurface.fillRGB(0,0,0);
    mGameSfc.fillRGB(0,0,0);
}

#if SDL_VERSION_ATLEAST(2, 0, 0)

bool mHack = false;

SDL_Texture *hackTex;

void CSDLVideo::hackIt()
{
    if(mHack) return;

    mHack = true;


     SDL_Surface* loadedSurface = IMG_Load( "global/dpad.png" );

     hackTex = SDL_CreateTextureFromSurface( renderer, loadedSurface );


     SDL_FreeSurface( loadedSurface );
}
#else
void CSDLVideo::hackIt() {} // Empty
#endif

void CSDLVideo::transformScreenToDisplay()
{

#if SDL_VERSION_ATLEAST(2, 0, 0)            
    mpScreenSfc->lock();
    SDL_UpdateTexture(mpSDLScreenTexture.get(), nullptr, mpScreenSfc->getSDLSurface()->pixels, mpScreenSfc->width() * sizeof (Uint32));
    mpScreenSfc->unlock();


    SDL_SetRenderDrawColor(renderer, mClearColor.r, mClearColor.g, mClearColor.b, 255);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, mpSDLScreenTexture.get(), nullptr, nullptr);

    // Now render the textures which additionally sent over...
    while(!mRenderTexturePtrs.empty())
    {
        auto &triple = mRenderTexturePtrs.front();

        GsTexture &gsTexture = std::get<0>(triple);
        SDL_Texture *texture = gsTexture.getPtr();
        const GsRect<Uint16> &src = std::get<1>(triple);
        const GsRect<Uint16> &dst = std::get<2>(triple);

        if(src.empty())
        {
            if(dst.empty())
            {
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            }
            else
            {
                SDL_Rect dstSDL = dst.SDLRect();
                SDL_RenderCopy(renderer, texture, nullptr, &dstSDL);
            }
        }
        else
        {
            SDL_Rect srcSDL = src.SDLRect();
            if(dst.empty())
            {
                SDL_RenderCopy(renderer, texture, &srcSDL, nullptr);
            }
            else
            {
                SDL_Rect dstSDL = dst.SDLRect();
                SDL_RenderCopy(renderer, texture, &srcSDL, &dstSDL);
            }
        }

        mRenderTexturePtrs.pop();
    }


    SDL_RenderPresent(renderer);


#else

    // Blit the stuff
    mpScreenSfc->blitScaledTo(mDisplaySfc);

    // Flip the screen (We use double-buffering on some systems.)
    mDisplaySfc.flip();

#endif

}
