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

    CScrollEffect(SDL_Surface *pScrollSurface,
            const Sint16 initialPos, Sint8 speed, const direction_t hDir, const direction_t vDir);

    CScrollEffect(GsWeakSurface &scrollSurface,
            const Sint16 initialPos, Sint8 speed, const direction_t hDir, const direction_t vDir);

    void ponder(const float deltaT);
    void render();

	Sint16 getScrollPosition();

private:

    Sint16 mInitSpeed;
    Sint16 mSpeed;
    Sint16 mInitialSpeed;
	Sint16 mScrollPos;
    const direction_t mHDir;
    const direction_t mVDir;
    std::shared_ptr<SDL_Surface> mpOldSurface;
    std::shared_ptr<SDL_Surface> mpScrollSurface;

    float mRemains = 0.0f;
};

#endif /* CSCROLLEFFECT_H_ */
