/*
 * CScrollEffect.h
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#ifndef CSCROLLEFFECT_H_
#define CSCROLLEFFECT_H_

#include <SDL.h>

#include "CEffects.h"

class CScrollEffect : public CEffects
{
public:
	CScrollEffect(SDL_Surface *pScrollSurface, SDL_Surface *pBackground,
			const Sint16 initialPos, Sint8 speed);

	void process();

	Sint16 getScrollPosition();

	virtual ~CScrollEffect();

private:

	Sint8 m_Speed;
	Sint16 m_ScrollPos;
	SDL_Surface *mp_OldSurface;
	SDL_Surface *mp_ScrollSurface;
};

#endif /* CSCROLLEFFECT_H_ */
