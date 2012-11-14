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
mSpeed(speed),
mInitialSpeed(speed),
mScrollPos(initialPos),
mpScrollSurface(pScrollSurface)
{
	mpOldSurface.reset( SDL_DisplayFormat(pBackground), &SDL_FreeSurface );
}

void CScrollEffect::process()
{
	if(mSpeed < 0)
	{
		mScrollPos += mSpeed;
		if(mScrollPos + mSpeed < 0) mScrollPos = 0;

		if(mScrollPos == 0) mFinished = true;
	}
	else
	{
		mScrollPos += mSpeed;
		if(mScrollPos  > mpOldSurface->h)
			mScrollPos = mpScrollSurface->h;

		if(mScrollPos+mSpeed >= mpScrollSurface->h) mFinished = true;
	}

	SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
	SDL_Rect dest = gameres;
	SDL_Rect src = gameres;

	src.y = mpScrollSurface->h-mScrollPos;
	dest.h = mScrollPos;

	SDL_BlitSurface( mpScrollSurface, &src,
					 g_pVideoDriver->getBlitSurface(), &dest );

}

Sint16 CScrollEffect::getScrollPosition()
{
	return mScrollPos;
}

