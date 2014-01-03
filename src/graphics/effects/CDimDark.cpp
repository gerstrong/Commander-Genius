/*
 * CDimDark.cpp
 *
 *  Created on: 29.11.2012
 *      Author: gerstrong
 */

#include "CDimDark.h"
#include <base/video/CVideoDriver.h>

CDimDark::CDimDark(const Uint8 speed) :
m_Speed(speed),
m_Alpha(0),
dimDark(true)
{
    gVideoDriver.collectSurfaces();
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    mpOldSurface.reset( gVideoDriver.convertThroughBlitSfc( gVideoDriver.mpVideoEngine->getBlitSurface() ), &SDL_FreeSurface );
    //mpDarkSurface.reset( gVideoDriver.convertThroughBlitSfc( gVideoDriver.mpVideoEngine->getBlitSurface() ), &SDL_FreeSurface );
//#endif

    auto *blit = gVideoDriver.getBlitSurface();
    SDL_PixelFormat *format = blit->format;

    SDL_Surface *sfc = SDL_CreateRGBSurface( SDL_SWSURFACE,
                blit->w, blit->h, RES_BPP,
                format->Rmask,
                format->Gmask,
                format->Bmask,
                0 );

    mpDarkSurface.reset( sfc, &SDL_FreeSurface );

    SDL_FillRect( mpDarkSurface.get(), NULL, SDL_MapRGB(mpDarkSurface->format, 0, 0, 0) );
}

void CDimDark::ponder(const float deltaT)
{
    if(dimDark) // dim dark here
    {

#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod( mpDarkSurface.get(), m_Alpha );
#else
        SDL_SetAlpha( mpDarkSurface.get(), SDL_SRCALPHA, m_Alpha );
#endif


        const int sum = m_Alpha + m_Speed;

        if(sum > 255)
        {
            m_Alpha = 0;
            dimDark = false;
        }
        else
        {
            m_Alpha += m_Speed;
        }
    }
    else // Undim the upcoming surface.
    {
        // Process the effect
#if SDL_VERSION_ATLEAST(2, 0, 0)
        SDL_SetSurfaceAlphaMod( mpDarkSurface.get(), 255-m_Alpha );
#else
        SDL_SetAlpha( mpDarkSurface.get(), SDL_SRCALPHA, 255-m_Alpha );
#endif

        const int sum = m_Alpha + m_Speed;

        if(sum > 255)
        {
            m_Alpha = 255;
            mFinished = true;
        }
        else
        {
            m_Alpha += m_Speed;
        }
    }
}

void CDimDark::render()
{
    if(dimDark) // dim dark here
    {
        // Process the effect
        SDL_BlitSurface( mpOldSurface.get(), NULL,
                         gVideoDriver.getBlitSurface(), NULL );

        SDL_BlitSurface( mpDarkSurface.get(), NULL,
                         gVideoDriver.getBlitSurface(), NULL );

    }
    else // Undim the upcoming surface.
    {

        SDL_BlitSurface( mpDarkSurface.get(), NULL,
                         gVideoDriver.getBlitSurface(), NULL );
    }
}

