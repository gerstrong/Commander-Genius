/*
 * CColorMerge.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 * This class gets two surface and blends them the way
 * the player will see a smooth scene transition.
 */

#ifndef CCOLORMERGE_H_
#define CCOLORMERGE_H_

#include <SDL.h>
#include <memory>

#include "CEffects.h"


class CColorMerge : public CEffects
{
public:
	CColorMerge(const Uint8 speed);

	void process();
	
	std::shared_ptr<SDL_Surface> &getSfc()
	{  return mpOldSurface;  }
	
	void getSnapshot();		

private:
	
	Uint8 m_Speed;
	Uint8 m_Alpha;

	std::shared_ptr<SDL_Surface> mpOldSurface;
};

#endif /* CCOLORMERGE_H_ */
