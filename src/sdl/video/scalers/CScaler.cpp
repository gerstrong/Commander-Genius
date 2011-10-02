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


/**
 * Scale functions
 */
void CScaler::scaleDynamic( SDL_Surface *srcSfc,
							SDL_Surface *dstSfc )
{
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
		for( xDst = 0; xDst<dstWidth ; xDst++ )
		{
			*dstPixel = srcPixel[pitch+Uint32(xSrc)];

			xSrc += l_wFac;
			dstPixel++;
		}

		ySrc += l_hFac;
	}

}



void CScaler::scaleUp(	SDL_Surface				*dstSfc,
						SDL_Surface				*srcSfc,
						const scaleOptionType	scaleOption )
{
	SDL_LockSurface( srcSfc );
	SDL_LockSurface( dstSfc );


	if( scaleOption == SCALEX && FilterFactor > 1 )
	{
		scale( 	FilterFactor,
				dstSfc->pixels,
				dstSfc->pitch,
				srcSfc->pixels,
				srcSfc->pitch,
				dstSfc->format->BytesPerPixel,
				srcSfc->w,
				srcSfc->h	);

	}
	else if( scaleOption == DYNAMIC )
	{

		scaleDynamic( srcSfc, dstSfc );

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



	SDL_UnlockSurface( dstSfc );
	SDL_UnlockSurface( srcSfc );
}


