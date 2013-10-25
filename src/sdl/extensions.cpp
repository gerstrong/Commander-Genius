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
#include "CLogFile.h"

#include <cstring>
#include <cstdio>

#if SDL_VERSION_ATLEAST(2, 0, 0)
#else
#include "SDL_rotozoom.h"
#endif

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


void drawEmptyRect(SDL_Surface *sfc, SDL_Rect *pRect, Uint32 color)
{
	SDL_Rect tempRect = *pRect;

	tempRect.h = 1;

	SDL_FillRect(sfc, &tempRect, color);

	tempRect.y = pRect->y + pRect->h - 1;

	SDL_FillRect(sfc, &tempRect, color);

	tempRect.w = 1;
	tempRect.h = pRect->h;
	tempRect.y = pRect->y;

	SDL_FillRect(sfc, &tempRect, color);

	tempRect.x = pRect->x + pRect->w - 1;

	SDL_FillRect(sfc, &tempRect, color);

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

int SDL_BlitScaledWrap(SDL_Surface *srcSfc, SDL_Rect *srcRect, SDL_Surface *dstSfc, SDL_Rect *dstRect )
{
    int error;

#if SDL_VERSION_ATLEAST(2, 0, 0)
    error = SDL_BlitScaled( srcSfc, srcRect, dstSfc, dstRect );
    if(error)
    {
        std::string errorMsg = SDL_GetError();
        g_pLogFile->textOut("SDL_Blit failed: " + errorMsg);
    }
#else

    double zoomx = dstRect->w/srcRect->w;
    double zoomy = dstRect->h/srcRect->h;

    // As SDL 1.2 doesn't support that, we need to do it here through Software with SDL_rotozoom.
    std::unique_ptr<SDL_Surface, SDL_Surface_Deleter>
            temp( zoomSurface( srcSfc, zoomx, zoomy, SMOOTHING_OFF ) );

    srcRect->w = dstRect->w;
    srcRect->h = dstRect->h;

    error = SDL_BlitSurface(temp.get(), srcRect, dstSfc, dstRect);

#endif

    return error;
}


SDL_Surface *CG_CreateRGBSurface( const SDL_Rect rect )
{
    auto *blit = g_pVideoDriver->getBlitSurface();
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

