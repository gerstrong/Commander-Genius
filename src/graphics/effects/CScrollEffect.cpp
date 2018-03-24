/*
 * CScrollEffect.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CScrollEffect.h"
#include <base/video/CVideoDriver.h>

#include <cassert>



CScrollEffect::CScrollEffect(GsWeakSurface &scrollSurface,
                             const int initialPos,
                             const int speed,
                             const direction_t hDir,
                             const direction_t vDir) :
mInitSpeed(speed),
mSpeed(speed),
mInitialSpeed(speed),
mScrollPos(initialPos),
mHDir(hDir),
mVDir(vDir)
{
    mScrollSurface.createCopy(scrollSurface);
}

void CScrollEffect::ponder(const float deltaT)
{
    float tdeltaLogic = mRemains + deltaT;
    mRemains = 0.0f;

    while(tdeltaLogic >= 1.0)
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
                posScrollSfc = mScrollSurface.height();
            }
            else
            {
                posOldSfc = gVideoDriver.getBlitSurface()->w;
                posScrollSfc = mScrollSurface.width();
            }


            if(mScrollPos  > posOldSfc)
                mScrollPos = posScrollSfc;

            if(mScrollPos+mSpeed >= posScrollSfc)
                mFinished = true;
        }

        tdeltaLogic -= 1.0f;
    }

    mRemains = tdeltaLogic;
}

void CScrollEffect::render()
{
    SDL_Rect gameres = gVideoDriver.getGameResolution().SDLRect();
    SDL_Rect dest = gameres;
    SDL_Rect src = mScrollSurface.getSDLSurface()->clip_rect;

    if(mVDir == DOWN)
    {
        src.y = mScrollSurface.height()-mScrollPos;
        dest.h = mScrollPos;
    }
    else if(mHDir == RIGHT)
    {
        src.x = mScrollSurface.width()-mScrollPos;
        dest.w = mScrollPos;
    }

    GsWeakSurface blitWeak(gVideoDriver.getBlitSurface());

    mScrollSurface.blitTo(blitWeak, src, dest);
}

int CScrollEffect::getScrollPosition()
{
	return mScrollPos;
}

