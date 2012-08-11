/*
 * CScaler.cpp
 *
 *  Created on: 01.10.2011
 *      Author: gerstrong
 */

#include "CScaler.h"
#include "scalebit.h"

/**
 * Setters
 */
void CScaler::setDynamicFactor( const float wFac, const float hFac)
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
}


/**
 * Scale functions
 */
void CScaler::scaleDynamic( SDL_Surface *srcSfc,
							SDL_Surface *dstSfc )
{
	const bool equalWidth  = (dstSfc->w == srcSfc->w);
	const bool equalHeight = (dstSfc->h == srcSfc->h);

	if(equalWidth && equalHeight)
	{
		SDL_BlitSurface(srcSfc, NULL, dstSfc, NULL);
		return;
	}

	const float dstWidth  = float(dstSfc->w);
	const float dstHeight = float(dstSfc->h);

	Uint32 *dstPixel = static_cast<Uint32*>(dstSfc->pixels);
	Uint32 *srcPixel = static_cast<Uint32*>(srcSfc->pixels);
	Uint32 pitch;

	// Pass those numbers to the stack
	const float l_wFac = wFac;
	const float l_hFac = hFac;

	float xSrc, ySrc;

	ySrc = 0.0f;
	for( Uint32 yDst = 0, xDst ; yDst<dstHeight ; yDst++ )
	{
		xSrc = 0.0f;

		pitch = Uint32(ySrc)*srcSfc->w;
		if(equalWidth)
		{
			memcpy(dstPixel, srcPixel+pitch, srcSfc->pitch);
			dstPixel += dstSfc->w;
		}
		else
		{

			for( xDst = 0; xDst<dstWidth ; xDst++ )
			{
				*dstPixel = srcPixel[pitch+Uint32(xSrc)];

				xSrc += l_wFac;
				dstPixel++;
			}
		}

		ySrc += l_hFac;
	}

}


// Software implementation of linear interpolation - too slow for us.
// The interpolation itself is based on code by Bruno Augier http://dzzd.net/
// See here for the code: http://www.java-gaming.org/index.php?topic=22121.0
void CScaler::scaleDynamicLinear( SDL_Surface *srcSfc,
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

}



// Another implementation of linear interpolation
// The interpolation itself is based on code by Bruno Augier http://dzzd.net/
// See here for the code: http://www.java-gaming.org/index.php?topic=22121.0
/*
void CScaler::scaleDynamicLinear( SDL_Surface *srcSfc,
							SDL_Surface *dstSfc )
{
	if((dstSfc->w == srcSfc->w) && (dstSfc->h == srcSfc->h))
	{
		SDL_BlitSurface(srcSfc, NULL, dstSfc, NULL);
		return;
	}

	const Uint32 srcWidth  = Uint32(srcSfc->w);
	const Uint32 srcHeight = Uint32(srcSfc->h);
	const Uint32 srcWidthMinusOne  = srcWidth-1;
	const Uint32 srcHeightMinusOne = srcHeight-1;
	const Uint32 dstWidth  = Uint32(dstSfc->w);
	const Uint32 dstHeight = Uint32(dstSfc->h);

	Uint32 *dstPixel = static_cast<Uint32*>(dstSfc->pixels), *currDstPixel;
	Uint32 *srcPixel = static_cast<Uint32*>(srcSfc->pixels);

	Uint32 *topLeftPixel = srcPixel, *topRightPixel, *bottomLeftPixel, *bottomRightPixel;
	Uint32 blackPixel[] = {0};

	Uint32 xDst, xDstMin, xDstMax, xDstDiff, yDst, yDstMin, yDstMax, yDstDiff;
	yDstMax = 0;

	Uint8 bX, bY, f24, f23, f14, f13;

	for( Uint32 ySrc = 0, xSrc ; ySrc<srcHeight ; ySrc++ )
	{
		yDstMin = yDstMax;
		yDstMax = (ySrc+1)*dstHeight/srcHeight;
		yDstDiff = yDstMax - yDstMin;

		xDstMax = 0;
		for( xSrc = 0 ; xSrc<srcWidth ; xSrc++ )
		{
			xDstMin = xDstMax;
			xDstMax = (xSrc+1)*dstWidth/srcWidth;
			xDstDiff = xDstMax - xDstMin;
			if (xSrc < srcWidthMinusOne)
			{
				topRightPixel = topLeftPixel+1;
				if (ySrc < srcHeightMinusOne)
				{
					bottomLeftPixel = topLeftPixel+srcWidth;
					bottomRightPixel = bottomLeftPixel+1;
				}
				else
					bottomLeftPixel = bottomRightPixel = blackPixel;
			}
			else
			{
				topRightPixel = bottomRightPixel = blackPixel;
				bottomLeftPixel = (ySrc < srcHeightMinusOne) ? topLeftPixel+srcWidth : blackPixel;
			}
			// TODO: What if yDstMax == yDstMin???? Or something
			currDstPixel = dstPixel + yDstMin*dstWidth + xDstMin;
			for ( yDst = yDstMin ; yDst < yDstMax ; yDst++ )
			{
				bY = Uint8(((yDst-yDstMin)<<8)/yDstDiff);
				for ( xDst = xDstMin ; xDst < xDstMax ; xDst++ )
				{
					bX = Uint8(((xDst-xDstMin)<<8)/xDstDiff);
					f24=(bX*bY)>>8;
					f23=bX-f24;
					f14=bY-f24;
					f13=((255-bX)*(255-bY))>>8;

					*currDstPixel = ((((*topLeftPixel&0xFF00FF)*f13+(*topRightPixel&0xFF00FF)*f23+(*bottomLeftPixel&0xFF00FF)*f14+(*bottomRightPixel&0xFF00FF)*f24)&0xFF00FF00)|
					                 (((*topLeftPixel&0x00FF00)*f13+(*topRightPixel&0x00FF00)*f23+(*bottomLeftPixel&0x00FF00)*f14+(*bottomRightPixel&0x00FF00)*f24)&0x00FF0000))>>8;
//					                 (((*topLeftPixel&0xFF00FF00)*f13+(*topRightPixel&0xFF00FF00)*f23+(*bottomLeftPixel&0xFF00FF00)*f14+(*bottomRightPixel&0xFF00FF00)*f24)&0xFF00FF0000))>>8;

					currDstPixel++;
				}
				currDstPixel-=xDstDiff;
				currDstPixel+=dstWidth;
			}
			topLeftPixel++;
		}
	}
}
*/



void CScaler::scaleNormal( SDL_Surface *srcSfc,
							SDL_Surface *dstSfc )
{
	if((dstSfc->w == srcSfc->w) && (dstSfc->h == srcSfc->h))
	{
		SDL_BlitSurface(srcSfc, NULL, dstSfc, NULL);
		return;
	}

	const Uint32 srcWidth  = Uint32(srcSfc->w);
	const Uint32 srcHeight = Uint32(srcSfc->h);
	const Uint32 dstWidth  = Uint32(dstSfc->w);
	const Uint16 dstPitch  = dstSfc->pitch;

	Uint32 *dstPixel = static_cast<Uint32*>(dstSfc->pixels);
	*origDstPixel = dstPixel;
	Uint32 *srcPixel = static_cast<Uint32*>(srcSfc->pixels);
	Uint32 zoomIndex;

	for( Uint32 ySrc = 0, xSrc ; ySrc<srcHeight ; ySrc++ )
	{
		// First we just stretch a row horizontally
		for( xSrc = 0 ; xSrc<srcWidth ; xSrc++, srcPixel++ )
			for( zoomIndex = 0; zoomIndex < FilterFactor; zoomIndex++, dstPixel++ )
				*dstPixel = *srcPixel;
		// Now we make copies of the stretched row
		for( zoomIndex = 1; zoomIndex < FilterFactor; zoomIndex++, dstPixel+=dstWidth )
			memcpy(dstPixel, origDstPixel, dstPitch);
		origDstPixel += zoomIndex*dstWidth;
	}
}



void CScaler::scaleUp(	SDL_Surface				*dstSfc,
						SDL_Surface				*srcSfc,
						const scaleOptionType	scaleOption )
{
	if( scaleOption == SCALEX && FilterFactor > 1 )
	{

		SDL_LockSurface( srcSfc );
		SDL_LockSurface( dstSfc );

		if (IsFilterNormal)
			scaleNormal( srcSfc, dstSfc );
		else
			scale( 	FilterFactor,
					dstSfc->pixels,
					dstSfc->pitch,
					srcSfc->pixels,
					srcSfc->pitch,
					dstSfc->format->BytesPerPixel,
					srcSfc->w,
					srcSfc->h	);

		SDL_UnlockSurface( dstSfc );
		SDL_UnlockSurface( srcSfc );
	}
	else if( scaleOption == DYNAMIC )
	{
		SDL_LockSurface( srcSfc );
		SDL_LockSurface( dstSfc );

		scaleDynamic( srcSfc, dstSfc );

		SDL_UnlockSurface( dstSfc );
		SDL_UnlockSurface( srcSfc );
	}
	else
	{

		SDL_Rect scrrect, dstrect;
		dstrect.x = scrrect.y = 0;
		dstrect.y = scrrect.x = 0;
		dstrect.h = scrrect.h = srcSfc->h;
		dstrect.w = scrrect.w = srcSfc->w;

		SDL_BlitSurface(srcSfc, &scrrect, dstSfc, &dstrect);

	}
}


