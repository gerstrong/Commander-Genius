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

#include "CEffects.h"


class CScrollEffect : public CEffects
{
public:
	CScrollEffect(SDL_Surface *pScrollSurface, SDL_Surface *pBackground,
                  const Sint16 initialPos, Sint8 speed);
    
	void process();
    
	Sint16 getScrollPosition();
    
private:
    
	Sint8 mSpeed;
	Sint8 mInitialSpeed;
	Sint16 mScrollPos;
	std::shared_ptr<SDL_Surface> mpOldSurface;
	SDL_Surface *mpScrollSurface;
};

#endif /* CSCROLLEFFECT_H_ */
