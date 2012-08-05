/*
 * CScaler.h
 *
 *  Created on: 01.10.2011
 *      Author: gerstrong
 */

#ifndef CSCALER_H_
#define CSCALER_H_

#include <SDL.h>

enum scaleOptionType
{
	NO_SCALE,
	SCALEX,
	DYNAMIC
};

class CScaler
{

public:

	CScaler() : wFac(1.0f), hFac(1.0f) {}

	/**
	 * \description This functions takes care of the scaling one surface to another
	 * \param		dstSfc	Pointer the destination surface
	 * \param		srcSfc	Pointer the source surface
	 * \param		scaleOptionType	Type of scale you want to perform
	 * 				NONE 	= Just blit over it
	 * 				SCALEX 	= use ScaleX
	 * 				DYNAMIC = Use the Software Scaler for window resizal recommended
	 */
	void scaleUp(	SDL_Surface				*dstSfc,
					SDL_Surface				*srcSfc,
					const scaleOptionType	scaleOption);


	void setDynamicFactor( const float wFac, const float hFac);

	void setFilterFactor( const Uint32 FilterFactor );

	Uint32 filterFactor() { return FilterFactor; }

	void setFilterType( bool IsNormal );

	//bool filterType() { return IsFilterNormal; }

private:

	void scaleDynamicLinear( SDL_Surface *srcSfc,
								SDL_Surface *dstSfc );

	void scaleDynamic( 	SDL_Surface *srcSfc,
						SDL_Surface *dstSfc );
	void scaleNormal( 	SDL_Surface *srcSfc,
						SDL_Surface *dstSfc );

	float 	wFac;
	float 	hFac;
	Uint32 	FilterFactor;
	bool    IsFilterNormal;
};

#endif /* CSCALER_H_ */
