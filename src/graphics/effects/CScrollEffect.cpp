/*
 * CScrollEffect.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CScrollEffect.h"
#include "sdl/CVideoDriver.h"

#include <cassert>


CScrollEffect::CScrollEffect(SDL_Surface *pScrollSurface,
                             const Sint16 initialPos, Sint8 speed,
                             const direction_t hDir, const direction_t vDir) :
mInitSpeed(speed),
mSpeed(2*speed),
mInitialSpeed(speed),
mScrollPos(initialPos),
mHDir(hDir),
mVDir(vDir)
{
    mpScrollSurface.reset( g_pVideoDriver->convertThroughBlitSfc(pScrollSurface), &SDL_FreeSurface );
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


        int posOldSfc, posScrollSfc;

        if(mVDir == DOWN)
        {
            posOldSfc = g_pVideoDriver->getBlitSurface()->h;
            posScrollSfc = mpScrollSurface->h;
        }
        else if(mHDir == RIGHT)
        {
            posOldSfc = g_pVideoDriver->getBlitSurface()->w;
            posScrollSfc = mpScrollSurface->w;
        }


        if(mScrollPos  > posOldSfc)
            mScrollPos = posScrollSfc;

        if(mScrollPos+mSpeed >= posScrollSfc)
            mFinished = true;
	}
}

void CScrollEffect::render()
{
    SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
    SDL_Rect dest = gameres;
    SDL_Rect src = mpScrollSurface->clip_rect;

    const int scaleFac = gameres.h/200;

    if(mVDir == DOWN)
    {
        src.y = mpScrollSurface->h-mScrollPos;
        dest.h = mScrollPos*scaleFac;
    }
    else if(mHDir == RIGHT)
    {
        src.x = mpScrollSurface->w-mScrollPos;
        dest.w = mScrollPos*scaleFac;
    }

    blitScaled(mpScrollSurface.get(),
               src,
               g_pVideoDriver->getBlitSurface(),
               dest,
               NONE);

}

Sint16 CScrollEffect::getScrollPosition()
{
	return mScrollPos;
}

