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
    mpOldSurface.reset( SDL_DisplayFormat( g_pVideoDriver->mpVideoEngine->getBlitSurface() ), &SDL_FreeSurface );
}

// Effect cycle
void CColorMerge::process()
{
	// Process the effect
	SDL_SetAlpha( mpOldSurface.get(), SDL_SRCALPHA, 255-m_Alpha );

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
