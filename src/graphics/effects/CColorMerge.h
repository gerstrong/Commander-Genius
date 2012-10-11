/*
 * CColorMerge.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 * This class gets two surface and blends them the way
 * the player will see a smooth scene transition.
 * NOTE: Rember to always use the Source and TargetSurfaces
 * If you don't the effect won't work!
 */

#ifndef CCOLORMERGE_H_
#define CCOLORMERGE_H_

#include <SDL.h>

#include "CEffects.h"
#include "SmartPointer.h"
#include "graphics/CBitmap.h"
#include "sdl/CTimer.h"


class CColorMerge : public CEffects
{
public:
	CColorMerge(const Uint8 speed, const SDL_Rect &cutRect);
	CColorMerge(const Uint8 speed);

	void process();
	
	void setCutRect(const SDL_Rect &cutRect);
	SmartPointer<SDL_Surface> &getSfc()
	{  return mpOldSurface;  }

private:
	void getSnapshot();		

	Uint8 m_Speed;
	Uint8 m_Alpha;
	CTimer mTimer;

	SmartPointer<SDL_Surface> mpOldSurface;
	SDL_Rect mCutRect;
};

#endif /* CCOLORMERGE_H_ */
