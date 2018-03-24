/*
 * CScaler.h
 *
 *  Created on: 01.10.2011
 *      Author: gerstrong
 */

#ifndef CSCALER_H_
#define CSCALER_H_

#include <SDL.h>
#include <base/utils/Geometry.h>

enum filterOptionType
{
    NONE = 1,
    SCALE_2X = 2,
    SCALE_3X = 3,
    SCALE_4X = 4
};

void blitScaled(SDL_Surface *srcSfc,
                      SDL_Rect &srGsRect,
                      SDL_Surface *dstSfc,
                      SDL_Rect &dstRect,
                      filterOptionType filter);

#endif /* CSCALER_H_ */
