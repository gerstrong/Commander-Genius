/*
 * CColorMerge.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 *
 */

#include "CColorMerge.h"
#include "sdl/CVideoDriver.h"

CColorMerge::CColorMerge(Uint8 speed) :
m_Speed(speed),
m_Alpha(0)
{
	getSnapshot();
	mTimer.ResetSecondsTimer();
}

// use this function. If you don't that, the effect won't work.
void CColorMerge::getSnapshot()
{
	g_pVideoDriver->collectSurfaces();
	mpOldSurface = SDL_DisplayFormat( g_pVideoDriver->mp_VideoEngine->getBlitSurface() );
}

// Effect cycle
void CColorMerge::process()
{
	// Process the effect
	SDL_SetAlpha( mpOldSurface.get(), SDL_SRCALPHA, 255-m_Alpha );

	SDL_BlitSurface( mpOldSurface.get(), NULL,
					 g_pVideoDriver->getBlitSurface(), NULL );

	if(m_Alpha + m_Speed > 255) m_Alpha = 255;
	else m_Alpha += m_Speed;

	if(m_Alpha == 255) mFinished = true;

}
