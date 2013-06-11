/*
 * CDimDark.cpp
 *
 *  Created on: 29.11.2012
 *      Author: gerstrong
 *  This darken the whole screen with alpha and lighten up the upcoming one.
 */

#ifndef CDIMDARK_H
#define CDIMDARK_H

#include "CEffects.h"
#include <memory>

class CDimDark : public CEffects
{

public:
    CDimDark(const Uint8 speed);
    void ponder();
    void render();
    
    std::shared_ptr<SDL_Surface> &getDarkSfc()
    {  return mpDarkSurface;  }
    
    std::shared_ptr<SDL_Surface> &getSfc()
    {  return mpOldSurface;  }

    
private:
	
	Uint8 m_Speed;
	Uint8 m_Alpha;
	bool dimDark;

	std::shared_ptr<SDL_Surface> mpOldSurface;    
	std::shared_ptr<SDL_Surface> mpDarkSurface;
};

#endif // CDIMDARK_H
