/*
 * CColorMerge.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#include "CColorMerge.h"
#include <base/video/CVideoDriver.h>


CColorMerge::CColorMerge(const Uint8 speed) :
m_Speed(speed),
m_Alpha(0)
{
    getSnapshot();
}

// use this function. If you don't that, the effect won't work.
void CColorMerge::getSnapshot()
{
    gVideoDriver.collectSurfaces();
//#if SDL_VERSION_ATLEAST(2, 0, 0)
    
//#else
    mpOldSurface.reset( gVideoDriver.convertThroughBlitSfc( gVideoDriver.mpVideoEngine->getBlitSurface() ), &SDL_FreeSurface );
//#endif        

}

// Effect cycle
void CColorMerge::ponder(const float deltaT)
{
	// Process the effect
#if SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_SetSurfaceAlphaMod( mpOldSurface.get(), 255-m_Alpha );
#else
    SDL_SetAlpha( mpOldSurface.get(), SDL_SRCALPHA, 255-m_Alpha );
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

void CColorMerge::render()
{
    BlitSurface( mpOldSurface.get(), NULL,
                gVideoDriver.getBlitSurface(), NULL );
}


