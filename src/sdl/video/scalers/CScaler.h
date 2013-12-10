/*
 * CScaler.h
 *
 *  Created on: 01.10.2011
 *      Author: gerstrong
 */

#ifndef CSCALER_H_
#define CSCALER_H_

#include <SDL.h>
#include <utils/Geometry.h>

enum filterOptionType
{
    NONE = 1,
    SCALE_2X = 2,
    SCALE_3X = 3,
    SCALE_4X = 4
};



class CScaler
{

public:

    CScaler(SDL_Surface *srcSfc,
            SDL_Surface *dstSfc,
            filterOptionType scaleOption) :
        mpSrcSfc(srcSfc),
        mpDstSfc(dstSfc),
        mFilter(scaleOption)
        {}

	/**
	 * \description This functions takes care of the scaling one surface to another
	 * \param		dstSfc	Pointer the destination surface
	 * \param		srcSfc	Pointer the source surface
	 * \param		scaleOptionType	Type of scale you want to perform
	 * 				NONE 	= Just blit over it
	 * 				SCALEX 	= use ScaleX
	 * 				DYNAMIC = Use the Software Scaler for window resizal recommended
	 */
    void process();


	void setDynamicFactor( const float wFac, const float hFac);

	void setFilterFactor( const Uint32 FilterFactor );

//	Uint32 filterFactor() { return FilterFactor; }

	void setFilterType( bool IsNormal );

	//bool filterType() { return IsFilterNormal; }

private:

	void scaleDynamicLinear( SDL_Surface *srcSfc,
                             SDL_Surface *dstSfc );

    void scaleDynamic( SDL_Surface *srcSfc,
                       SDL_Rect &srcRect,
                       SDL_Surface *dstSfc,
                       SDL_Rect &dstRect );

    SDL_Surface *mpSrcSfc;
    SDL_Surface *mpDstSfc;
    filterOptionType mFilter;
};

#endif /* CSCALER_H_ */
