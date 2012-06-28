/*
 * extensions.h
 *
 *  Created on: 31.01.2012
 *      Author: gerstrong
 *  Extensions for SDL
 */

#ifndef SDL_EXTENSIONS_H_
#define SDL_EXTENSIONS_H_

#include <SDL.h>

struct Colormask
{
	 Uint32 r,g,b,a;
};

Uint32 getPixel(SDL_Surface *surface, int x, int y);

inline Colormask getColourMask32bit();

SDL_Surface *CG_CreateRGBSurface( const SDL_Rect rect );

void drawEmptyRect(SDL_Surface *sfc, SDL_Rect *pRect, Uint32 color);


#endif /* SDL_EXTENSIONS_H_ */
