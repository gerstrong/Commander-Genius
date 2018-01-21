/*
 * CScrollEffect.h
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#ifndef CSCROLLEFFECT_H_
#define CSCROLLEFFECT_H_

#include <SDL.h>
#include <memory>

#include <base/video/GsEffectController.h>
#include <graphics/GsSurface.h>

#include "engine/core/direction.h"


class CScrollEffect : public CEffects
{
public:        

    CScrollEffect(GsWeakSurface &scrollSurface,
                  const int initialPos,
                  const int speed,
                  const direction_t hDir,
                  const direction_t vDir);

    void ponder(const float deltaT);
    void render();

    int getScrollPosition();

private:

    int mInitSpeed = 0;
    int mSpeed = 0;
    int mInitialSpeed = 0;
    int mScrollPos = 0;
    const direction_t mHDir;
    const direction_t mVDir;
    GsSurface mOldSurface, mScrollSurface;

    float mRemains = 0.0f;
};

#endif /* CSCROLLEFFECT_H_ */
