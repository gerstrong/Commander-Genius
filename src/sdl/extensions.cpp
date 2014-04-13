/*
 * extensions.cpp
 *
 *  Created on: 31.01.2012
 *      Author: gerstrong
 *
 *  Extensions for SDL
 */

#include "extensions.h"

#include <base/video/CVideoDriver.h>
#include <base/GsLogging.h>

#include <cstring>
#include <cstdio>

#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
//#include "SDL_rotozoom.h"
#endif




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
    auto *blit = gVideoDriver.getBlitSurface();
    SDL_PixelFormat *format = blit->format;

    SDL_Surface *sfc = SDL_CreateRGBSurface( SDL_SWSURFACE,
                rect.w,
                rect.h,
                RES_BPP,
                format->Rmask,
                format->Gmask,
                format->Bmask,
                format->Amask );

	return sfc;
}

