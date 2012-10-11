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

CColorMerge::CColorMerge(const Uint8 speed, const SDL_Rect &cutRect) :
m_Speed(speed),
m_Alpha(0)
{
    getSnapshot();
    setCutRect(cutRect);
}


CColorMerge::CColorMerge(const Uint8 speed) :
m_Speed(speed),
m_Alpha(0)
{
    mCutRect.x = mCutRect.y = 0; 
    mCutRect.w = mCutRect.h = 0; 

    getSnapshot();
    mTimer.ResetSecondsTimer();
}

// use this function. If you don't that, the effect won't work.
void CColorMerge::getSnapshot()
{
	g_pVideoDriver->collectSurfaces();
	//mpOldSurface = SDL_DisplayFormat( g_pVideoDriver->mp_VideoEngine->getBlitSurface() );
	mpOldSurface = SDL_DisplayFormat( g_pVideoDriver->mp_VideoEngine->getBlitSurface() );
}

void CColorMerge::setCutRect(const SDL_Rect &cutRect)
{
    mCutRect = cutRect;

    Uint32 colorkey = SDL_MapRGB( mpOldSurface->format, 0, 0xFF, 0xFF );
    SDL_SetColorKey( mpOldSurface.get(), SDL_SRCCOLORKEY, colorkey );

    // Cut out a Part where requested !!
    if(mCutRect.h != 0 && mCutRect.h != 0 )
    {
	SDL_FillRect( mpOldSurface.get(), &mCutRect, colorkey );
    }
    
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
