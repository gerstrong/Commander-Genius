/*
 * CScaler.cpp
 *
 *  Created on: 01.10.2011
 *      Author: gerstrong
 */

#include "CScaler.h"
#include "scalebit.h"

#include <graphics/GsSurface.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

/**
 * Setters
 */
/*void CScaler::setDynamicFactor( const float wFac, const float hFac)
{
	this->hFac = hFac;
	this->wFac = wFac;
}


void CScaler::setFilterFactor( const Uint32 FilterFactor )
{
	this->FilterFactor = FilterFactor;
}

void CScaler::setFilterType( bool IsNormal )
{
	this->IsFilterNormal = IsNormal;
}*/


/**
 * Scale functions
 */
void scaleDynamic( SDL_Surface *srcSfc,
                   SDL_Rect srGsRect,
                   SDL_Surface *dstSfc,
                   SDL_Rect dstRect )
{
	const bool equalWidth  = (dstRect.w == srcSfc->w);
	const bool equalHeight = (dstRect.h == srcSfc->h);

	if(equalWidth && equalHeight)
	{
		SDL_Rect sdldstrect;
		sdldstrect.x = dstRect.x; sdldstrect.y = dstRect.y;
		sdldstrect.w = dstRect.w; sdldstrect.h = dstRect.h;

        assert(srcSfc);
        assert(dstSfc);
        BlitSurface(srcSfc, &srGsRect, dstSfc, &sdldstrect);
		return;
	}

    SDL_LockSurface( srcSfc );
    SDL_LockSurface( dstSfc );

    Uint32 *dstFirstPixel = static_cast<Uint32*>(dstSfc->pixels);
    Uint32 *srcFirstPixel = static_cast<Uint32*>(srcSfc->pixels);
    Uint32 *dstPixel = dstFirstPixel;
    Uint32 *srcPixel = srcFirstPixel;

    // Pass those numbers to the stack, they are used very often.
    const float wFac = (float(srcSfc->w)) / (float(dstRect.w));
    const float hFac = (float(srcSfc->h)) / (float(dstRect.h));

    int pitch = dstRect.y*dstSfc->w;


    if(pitch >= 0)
        dstPixel += pitch;

    if(dstRect.x >= 0)
        dstPixel += dstRect.x;

    if(dstRect.w > dstSfc->w)
        dstRect.w = dstSfc->w;

    if(dstRect.h > dstSfc->h)
        dstRect.h = dstSfc->h;

	float xSrc, ySrc;

    ySrc = 0.0f;

    //SDL_PixelFormat *format = srcSfc->format;


    if(equalWidth)
    {
        for( int yDst = 0 ; yDst<dstSfc->h ; yDst++ )
        {
            const Uint32 posY = (yDst*srcSfc->h)/dstSfc->h;
            memcpy(dstPixel, srcPixel+(posY*dstSfc->w), srcSfc->pitch);
            dstPixel += dstSfc->w;
        }
    }
    else
    {

        for( int yDst = 0 ; yDst<dstRect.h ; yDst++ )
        {
            xSrc = 0.0f;

            pitch = Uint32(ySrc)*srcSfc->w;

            for( int xDst = 0; xDst<dstRect.w ; xDst++ )
            {
                *dstPixel = srcPixel[pitch+Uint32(xSrc)];
                xSrc += wFac;
                dstPixel++;
            }

            ySrc += hFac;

            dstPixel = dstFirstPixel + (dstSfc->w*yDst);            
        }
    }

    SDL_UnlockSurface( dstSfc );
    SDL_UnlockSurface( srcSfc );
}


void scaleNormal( SDL_Surface *srcSfc,
                  SDL_Surface *dstSfc,
                  const Uint32 filterFac)
{
    if( (dstSfc->w == srcSfc->w) && (dstSfc->h == srcSfc->h) )
	{
        assert(srcSfc);
        assert(dstSfc);
        BlitSurface(srcSfc, NULL, dstSfc, NULL);
		return;
	}

	const Uint32 srcWidth  = Uint32(srcSfc->w);
	const Uint32 srcHeight = Uint32(srcSfc->h);
	const Uint32 dstWidth  = Uint32(dstSfc->w);
	const Uint16 dstPitch  = dstSfc->pitch;

	Uint32 *dstPixel = static_cast<Uint32*>(dstSfc->pixels);
	Uint32 *origDstPixel = dstPixel;
	Uint32 *srcPixel = static_cast<Uint32*>(srcSfc->pixels);
	Uint32 zoomIndex;

	for( Uint32 ySrc = 0, xSrc ; ySrc<srcHeight ; ySrc++ )
	{
		// First we just stretch a row horizontally
		for( xSrc = 0 ; xSrc<srcWidth ; xSrc++, srcPixel++ )
            for( zoomIndex = 0; zoomIndex < filterFac; zoomIndex++, dstPixel++ )
				*dstPixel = *srcPixel;
		// Maybe a POT surface is used
		dstPixel+=dstWidth-(dstPixel-origDstPixel);
		// Now we make copies of the stretched row
        for( zoomIndex = 1; zoomIndex < filterFac; zoomIndex++, dstPixel+=dstWidth )
			memcpy(dstPixel, origDstPixel, dstPitch);
		origDstPixel += zoomIndex*dstWidth;
	}
}


void blitScaled(SDL_Surface *srcSfc,
                      SDL_Rect &srGsRect,
                      SDL_Surface *dstSfc,
                      SDL_Rect &dstRect,
                      filterOptionType filter)
{            

    assert(filter>=NONE && filter<=SCALE_4X);

    // First phase: Filter the surface (scaleX)
    SDL_Rect lSrGsRect = srGsRect;
    SDL_Rect lDstRect = dstRect;

    // Check for filter and reduce if the surface to be scaled is way too small
    while( filter>NONE )
    {
        // Does it fit?
        if( dstSfc->w >= srcSfc->w*filter &&
            dstSfc->h >= srcSfc->h*filter )
        {
            break;
        }

        filter = (filterOptionType)((int)(filter)-1);
    }

    if( filter>NONE )
    {
        SDL_LockSurface( srcSfc );
        SDL_LockSurface( dstSfc );

        scale( filter,
               dstSfc->pixels,
               dstSfc->pitch,
               srcSfc->pixels,
               srcSfc->pitch,
               dstSfc->format->BytesPerPixel,
               srcSfc->w,
               srcSfc->h );

        SDL_UnlockSurface( dstSfc );
        SDL_UnlockSurface( srcSfc );

        lSrGsRect.w = lSrGsRect.w*filter;
        lSrGsRect.h = lSrGsRect.h*filter;
    }

    // Second phase: Scale it normally
#if SDL_VERSION_ATLEAST(2, 0, 0)

    SDL_BlitScaled( srcSfc, &lSrGsRect, dstSfc, &lDstRect );

#else

    scaleDynamic( srcSfc, lSrGsRect, dstSfc, lDstRect );
    //scaleNormal(srcSfc, dstSfc, (Uint32)SCALE_4X );

#endif

}
