/*
 * CDimDark.cpp
 *
 *  Created on: 29.11.2012
 *      Author: gerstrong
 */

#include "CDimDark.h"
#include "sdl/CVideoDriver.h"

CDimDark::CDimDark(const Uint8 speed) :
m_Speed(speed),
m_Alpha(0),
dimDark(true)
{
    g_pVideoDriver->collectSurfaces();
    mpOldSurface.reset( SDL_DisplayFormat( g_pVideoDriver->mpVideoEngine->getBlitSurface() ), &SDL_FreeSurface );
    mpDarkSurface.reset( SDL_DisplayFormat( g_pVideoDriver->mpVideoEngine->getBlitSurface() ), &SDL_FreeSurface );
    SDL_FillRect( mpDarkSurface.get(), NULL, 0x0 );
}

void CDimDark::process()
{
    if(dimDark) // dim dark here
    {
        // Process the effect
        SDL_BlitSurface( mpOldSurface.get(), NULL,
                        g_pVideoDriver->getBlitSurface(), NULL );
        
        SDL_SetAlpha( mpDarkSurface.get(), SDL_SRCALPHA, m_Alpha );
        
        SDL_BlitSurface( mpDarkSurface.get(), NULL,
                        g_pVideoDriver->getBlitSurface(), NULL );
        
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
        SDL_SetAlpha( mpDarkSurface.get(), SDL_SRCALPHA, 255-m_Alpha );
        
        SDL_BlitSurface( mpDarkSurface.get(), NULL,
                        g_pVideoDriver->getBlitSurface(), NULL );
        
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

