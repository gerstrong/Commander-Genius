/*
 * CColorMerge.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#include "CColorMerge.h"
#include "sdl/CVideoDriver.h"


CColorMerge::CColorMerge(const Uint8 speed) :
m_Speed(speed),
m_Alpha(0)
{
    getSnapshot();
}

// use this function. If you don't that, the effect won't work.
void CColorMerge::getSnapshot()
{
    g_pVideoDriver->collectSurfaces();
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    mpOldSurface.reset( SDL_DisplayFormat( g_pVideoDriver->mpVideoEngine->getBlitSurface() ), &SDL_FreeSurface );
#endif
}

// Effect cycle
void CColorMerge::process()
{
	// Process the effect
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    SDL_SetAlpha( mpOldSurface.get(), SDL_SRCALPHA, 255-m_Alpha );
#endif

	SDL_BlitSurface( mpOldSurface.get(), NULL,
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
