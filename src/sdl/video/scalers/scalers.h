/*
 * scalers.h
 *
 *  Created on: 01.10.2011
 *      Author: gerstrong
 */

#ifndef SCALERS_H_
#define SCALERS_H_

#include <SDL/SDL.h>

enum scaleOptionType
{
	NONE,
	SCALEX,
	DYNAMIC
};


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


#endif /* SCALERS_H_ */
