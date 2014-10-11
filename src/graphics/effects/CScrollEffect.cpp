/*
 * CScrollEffect.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CScrollEffect.h"
#include <base/video/CVideoDriver.h>

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
    mpScrollSurface.reset( gVideoDriver.convertThroughBlitSfc(pScrollSurface), &SDL_FreeSurface );
}

CScrollEffect::CScrollEffect(GsWeakSurface &scrollSurface,
                             const Sint16 initialPos, Sint8 speed,
                             const direction_t hDir, const direction_t vDir) :
mInitSpeed(speed),
mSpeed(2*speed),
mInitialSpeed(speed),
mScrollPos(initialPos),
mHDir(hDir),
mVDir(vDir)
{
    mpScrollSurface.reset( gVideoDriver.convertThroughBlitSfc(scrollSurface.getSDLSurface()), &SDL_FreeSurface );
}

void CScrollEffect::ponder(const float deltaT)
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
            posOldSfc = gVideoDriver.getBlitSurface()->h;
            posScrollSfc = mpScrollSurface->h;
        }
        else
        {
            posOldSfc = gVideoDriver.getBlitSurface()->w;
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
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    SDL_Rect dest = gameres;
    SDL_Rect src = mpScrollSurface->clip_rect;

    if(mVDir == DOWN)
    {
        src.y = mpScrollSurface->h-mScrollPos;
        dest.h = mScrollPos;
    }
    else if(mHDir == RIGHT)
    {
        src.x = mpScrollSurface->w-mScrollPos;
        dest.w = mScrollPos;
    }

    BlitSurface(mpScrollSurface.get(),
                   &src,
                   gVideoDriver.getBlitSurface(),
                   &dest);

}

Sint16 CScrollEffect::getScrollPosition()
{
	return mScrollPos;
}

