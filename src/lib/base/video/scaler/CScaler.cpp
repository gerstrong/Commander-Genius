/*
 * CScaler.cpp
 *
 *  Created on: 01.10.2011
 *      Author: gerstrong
 */

#include "CScaler.h"
#include "scalebit.h"

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
        SDL_BlitSurface(srcSfc, &srGsRect, dstSfc, &sdldstrect);
		return;
	}

    //SDL_FillRect( dstSfc, nullptr, SDL_MapRGB(dstSfc->format, 0, 0, 0) );

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

    SDL_PixelFormat *format = srcSfc->format;


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
                const Uint32 newPixel = srcPixel[pitch+Uint32(xSrc)];

                Uint8 r, g, b, a;

                SDL_GetRGBA(newPixel, format,&r, &g, &b, &a);

                if(a > 0)
                {
                    *dstPixel = newPixel;
                }

                xSrc += wFac;
                dstPixel++;
            }

            dstPixel = dstFirstPixel + (dstSfc->w*yDst);

            ySrc = yDst*hFac;
        }
    }

    SDL_UnlockSurface( dstSfc );
    SDL_UnlockSurface( srcSfc );
}


// Software implementation of linear interpolation - too slow for us.
// The interpolation itself is based on code by Bruno Augier http://dzzd.net/
// See here for the code: http://www.java-gaming.org/index.php?topic=22121.0
/*void CScaler::scaleDynamicLinear( SDL_Surface *srcSfc,
							SDL_Surface *dstSfc )
{
	if((dstSfc->w == srcSfc->w) && (dstSfc->h == srcSfc->h))
	{
		SDL_BlitSurface(srcSfc, NULL, dstSfc, NULL);
		return;
	}

	const Uint32 srcWidthInt  = srcSfc->w;
	const float srcWidthMinusOne  = float(srcSfc->w-1);
	const float srcHeightMinusOne = float(srcSfc->h-1);
	const float dstWidth  = float(dstSfc->w);
	const float dstHeight = float(dstSfc->h);

	Uint32 *dstPixel = static_cast<Uint32*>(dstSfc->pixels);
	Uint32 *srcPixel = static_cast<Uint32*>(srcSfc->pixels);
	Uint32 pitch;

	Uint32 *topLeftPixel, *topRightPixel, *bottomLeftPixel, *bottomRightPixel;
	Uint32 blackPixel[] = {0};

	// Pass those numbers to the stack
	const float l_wFac = wFac;
	const float l_hFac = hFac;

	float xSrc, ySrc;
	Uint32 xSrcInt;

	//int channel;

	Uint8 bX, bY, f24, f23, f14, f13;
	//float xSmallWeight, ySmallWeight, yHighWeight;

	ySrc = 0.0f;
	for( Uint32 yDst = 0, xDst ; yDst<dstHeight ; yDst++ )
	{
		//ySrcInt = Uint32(ySrc);
		//ySmallWeight = ySrc-Uint32(ySrc);
		//yHighWeight  = 1 - ySmallWeight;
		bY = Uint8((ySrc-Uint32(ySrc))*256);
		xSrc = 0.0f;

		pitch = Uint32(ySrc)*srcWidthInt;
		for( xDst = 0; xDst<dstWidth ; xDst++ )
		{
			xSrcInt = Uint32(xSrc);
			//xSmallWeight = xSrc-xSrcInt;
			bX = Uint8((xSrc-xSrcInt)*256);
			f24=(bX*bY)>>8;
			f23=bX-f24;
			f14=bY-f24;
			f13=((255-bX)*(255-bY))>>8;
			topLeftPixel = srcPixel+pitch+xSrcInt;
			// We should take care of the right and bottom edges
			if (xSrc < srcWidthMinusOne)
			{
				topRightPixel = topLeftPixel+1;
				if (ySrc < srcHeightMinusOne)
				{
					bottomLeftPixel = topLeftPixel+srcWidthInt;
					bottomRightPixel = bottomLeftPixel+1;
				}
				else
					bottomLeftPixel = bottomRightPixel = blackPixel;
			}
			else
			{
				topRightPixel = bottomRightPixel = blackPixel;
				bottomLeftPixel = (ySrc < srcHeightMinusOne) ? topLeftPixel+srcWidthInt : blackPixel;
			}

//			*dstPixel = Uint32((1-xSmallWeight)*(*topLeftPixel*yHighWeight+*bottomLeftPixel*ySmallWeight)+
//			                   xSmallWeight*(*topRightPixel*yHighWeight+*bottomRightPixel*ySmallWeight));
			*dstPixel = ((((*topLeftPixel&0xFF00FF)*f13+(*topRightPixel&0xFF00FF)*f23+(*bottomLeftPixel&0xFF00FF)*f14+(*bottomRightPixel&0xFF00FF)*f24)&0xFF00FF00)|
			             (((*topLeftPixel&0x00FF00)*f13+(*topRightPixel&0x00FF00)*f23+(*bottomLeftPixel&0x00FF00)*f14+(*bottomRightPixel&0x00FF00)*f24)&0x00FF0000))>>8;
//			             (((*topLeftPixel&0xFF00FF00)*f13+(*topRightPixel&0xFF00FF00)*f23+(*bottomLeftPixel&0xFF00FF00)*f14+(*bottomRightPixel&0xFF00FF00)*f24)&0xFF00FF0000))>>8;

			xSrc += l_wFac;
			dstPixel++;
		}

		ySrc += l_hFac;
	}

}*/

/*void CScaler::scaleNormal( SDL_Surface *srcSfc,
                           SDL_Surface *dstSfc )
{
    if( (dstSfc->w == srcSfc->w) && (dstSfc->h == srcSfc->h) )
	{
		SDL_BlitSurface(srcSfc, NULL, dstSfc, NULL);
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
			for( zoomIndex = 0; zoomIndex < FilterFactor; zoomIndex++, dstPixel++ )
				*dstPixel = *srcPixel;
		// Maybe a POT surface is used
		dstPixel+=dstWidth-(dstPixel-origDstPixel);
		// Now we make copies of the stretched row
		for( zoomIndex = 1; zoomIndex < FilterFactor; zoomIndex++, dstPixel+=dstWidth )
			memcpy(dstPixel, origDstPixel, dstPitch);
		origDstPixel += zoomIndex*dstWidth;
	}
}
*/

void blitScaled(SDL_Surface *srcSfc,
                      SDL_Rect &srGsRect,
                      SDL_Surface *dstSfc,
                      SDL_Rect &dstRect,
                      filterOptionType filter)
{            

#ifdef DEBUG
    assert(filter>=NONE && filter<=SCALE_4X);
#endif

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

#endif

}
