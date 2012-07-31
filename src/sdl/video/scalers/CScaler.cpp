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
/*
void CScaler::scaleDynamicLinear( SDL_Surface *srcSfc,
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

	Uint32 *topLeftPixel, *topRightPixel, *bottomLeftPixel, *bottomRightPixel;

	// Pass those numbers to the stack
	const float l_wFac = wFac;
	const float l_hFac = hFac;

	float xSrc, ySrc;

	int channel;

	ySrc = 0.0f;
	for( Uint32 yDst = 0, xDst ; yDst<dstHeight ; yDst++ )
	{
		xSrc = 0.0f;

		pitch = Uint32(ySrc)*srcSfc->w;
		if(equalWidth)
		{
			memcpy(dstPixel, srcPixel+pitch, srcSfc->pitch);
		}
		else
		{
			for( xDst = 0; xDst<dstWidth ; xDst++ )
			{
				topLeftPixel = srcPixel+pitch+Uint32(xSrc);
				if (xSrc >= 1.0f)
					topLeftPixel -= 1;
				if (pitch >= srcSfc->w)
					topLeftPixel -= srcSfc->w;

				topRightPixel    = topLeftPixel+1;
				bottomLeftPixel  = topLeftPixel+srcSfc->w;
				bottomRightPixel = bottomLeftPixel+1;

				for (channel = 0; channel < 4; channel++)
					((Uint8 *)(dstPixel))[channel] =
						Uint8(
						((Uint8 *)topLeftPixel)[channel]*(1-xSrc+Uint32(xSrc))*(1-ySrc+Uint32(ySrc))+
						((Uint8 *)topRightPixel)[channel]*(xSrc-Uint32(xSrc))*(1-ySrc+Uint32(ySrc))+
						((Uint8 *)bottomLeftPixel)[channel]*(1-xSrc+Uint32(xSrc))*(ySrc-Uint32(ySrc))+
						((Uint8 *)bottomRightPixel)[channel]*(xSrc-Uint32(xSrc))*(ySrc-Uint32(ySrc))
						);

				xSrc += l_wFac;
				dstPixel++;
			}
		}

		ySrc += l_hFac;
	}

}
*/



// Another implementation of linear interpolation - slow again.
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
	const Uint32 dstWidth  = Uint32(dstSfc->w);
	const Uint32 dstHeight = Uint32(dstSfc->h);
	const Uint32 dstArea   = dstWidth*dstHeight;

	Uint32 *dstPixel = static_cast<Uint32*>(dstSfc->pixels);
	Uint32 *srcPixel = static_cast<Uint32*>(srcSfc->pixels);

	int channel;
	Uint32 leftEdge, topEdge;

	for( Uint32 yDst = 0, xDst ; yDst<dstHeight ; yDst++ )
		for( xDst = 0 ; xDst<dstWidth ; xDst++ )
		{
			leftEdge = xDst*srcWidth/dstWidth;
			if (leftEdge > 0)
				leftEdge--;
			topEdge  = yDst*srcHeight/dstHeight;
			if (topEdge > 0)
				topEdge--;
			for( channel = 0 ; channel < 4 ; channel++ )
			{
				((Uint8 *)(dstPixel))[channel] =
					(
					((Uint8 *)(srcPixel + (topEdge+1)*srcWidth+leftEdge+1))[channel]
					* (yDst*srcHeight%dstHeight) * (xDst*srcWidth%dstWidth) +
					((Uint8 *)(srcPixel + topEdge*srcWidth+leftEdge+1))[channel]
					* (dstHeight - yDst*srcHeight%dstHeight) * (xDst*srcWidth%dstWidth) +
					((Uint8 *)(srcPixel + (topEdge+1)*srcWidth+leftEdge))[channel]
					* (yDst*srcHeight%dstHeight) * (dstWidth - xDst*srcWidth%dstWidth) +
					((Uint8 *)(srcPixel + topEdge*srcWidth+leftEdge))[channel]
					* (dstHeight - yDst*srcHeight%dstHeight) * (dstWidth - xDst*srcWidth%dstWidth)
					) / dstArea;
			}
			dstPixel++;
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

	Uint32 *dstPixel = static_cast<Uint32*>(dstSfc->pixels), *origDstPixel = dstPixel;
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


