/*
 * extensions.cpp
 *
 *  Created on: 31.01.2012
 *      Author: gerstrong
 *
 *  Extensions for SDL
 */

#include "extensions.h"

#include "sdl/CVideoDriver.h"



Uint32 getPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp)
    {
    case 1:
    	return *p;
    	break;

    case 2:
    	return *(Uint16 *)p;
    	break;

    case 3:
    	if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
    		return p[0] << 16 | p[1] << 8 | p[2];
    	else
    		return p[0] | p[1] << 8 | p[2] << 16;
    	break;

    case 4:
    	return *(Uint32 *)p;
    	break;

    default:
    	return 0;       /* shouldn't happen, but avoids warnings */
    }
}




inline Colormask getColourMask32bit()
{
	Colormask mask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	mask.r = 0xFF000000;
	mask.g = 0x00FF0000;
	mask.b = 0x0000FF00;
	mask.a = 0x000000FF;
#else
	mask.r = 0x000000FF;
	mask.g = 0x0000FF00;
	mask.b = 0x00FF0000;
	mask.a = 0xFF000000;
#endif

	return mask;
}


SDL_Surface *CG_CreateRGBSurface( const SDL_Rect rect )
{
	Colormask mask = getColourMask32bit();

	SDL_Surface *sfc = SDL_CreateRGBSurface( SDL_SWSURFACE, // TODO: Must be changed in future for better hardware support
			rect.w,
			rect.h,
			32, // TODO: Must be changed in future for better 16 bpp support
			mask.r,
			mask.g,
			mask.b,
			mask.a );

	return sfc;
}

