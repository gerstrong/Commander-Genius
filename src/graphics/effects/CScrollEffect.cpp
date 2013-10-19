/*
 * CScrollEffect.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CScrollEffect.h"
#include "sdl/CVideoDriver.h"

CScrollEffect::CScrollEffect(SDL_Surface *pScrollSurface, SDL_Surface *pBackground,
							const Sint16 initialPos, Sint8 speed) :
mInitSpeed(speed),
mSpeed(2*speed),
mInitialSpeed(speed),
mScrollPos(initialPos),
mpScrollSurface(pScrollSurface)
{
    mpOldSurface.reset( g_pVideoDriver->convertThroughBlitSfc(pBackground), &SDL_FreeSurface );
}

void CScrollEffect::ponder()
{        
    if(mSpeed < 0)
	{
        if(mSpeed < -1)
            mSpeed++;

		mScrollPos += mSpeed;
		if(mScrollPos + mSpeed < 0) mScrollPos = 0;

        if(mScrollPos == 0)
            mFinished = true;
	}
	else
	{
        if(mSpeed > 1)
            mSpeed--;

		mScrollPos += mSpeed;
		if(mScrollPos  > mpOldSurface->h)
			mScrollPos = mpScrollSurface->h;

        if(mScrollPos+mSpeed >= mpScrollSurface->h)
            mFinished = true;
	}
}

void CScrollEffect::render()
{
    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
    SDL_Rect dest = gameres;
    SDL_Rect src = mpScrollSurface->clip_rect;

    const int scaleFac = gameres.h/200;

    src.y = mpScrollSurface->h-mScrollPos;
    dest.h = mScrollPos*scaleFac;

    SDL_BlitScaledWrap(mpScrollSurface,
                   &src,
                   g_pVideoDriver->getBlitSurface(),
                   &dest);
}

Sint16 CScrollEffect::getScrollPosition()
{
	return mScrollPos;
}

