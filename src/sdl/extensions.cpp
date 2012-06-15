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
    Uint32 pixColor = 0;

    switch(bpp)
    {
    case 1:
    	pixColor = *p;
    	break;

    case 2:
    	memcpy(&pixColor, p, sizeof(Uint16));
    	break;

    case 3:
    	if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
    		return p[0] << 16 | p[1] << 8 | p[2];
    	else
    		return p[0] | p[1] << 8 | p[2] << 16;
    	break;

    case 4:
    	memcpy(&pixColor, p, sizeof(Uint32));
    	break;

    default:
    	return 0;       /* shouldn't happen, but avoids warnings */
    }

    return pixColor;
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
	//SDL_Surface *video = SDL_GetVideoSurface();

	SDL_Surface *sfc = SDL_CreateRGBSurface( SDL_SWSURFACE,
			rect.w,
			rect.h,
			RES_BPP,
			mask.r,
			mask.g,
			mask.b,
			mask.a );


	/*SDL_Surface *sfc = SDL_CreateRGBSurface( SDL_SWSURFACE,
			rect.w,
			rect.h,
			RES_BPP,
			video->format->Rmask,
			video->format->Gmask,
			video->format->Bmask,
			video->format->Amask);*/

	return sfc;
}

