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

enum class VidFilter
{
    NONE = 1,
    SCALE_2X = 2,
    SCALE_3X = 3,
    SCALE_4X = 4
};

/**
 * @brief sFiltToNum converts the VidFilter type to a number
 *        good for cases where you compare or multiply
 * @param filt filter to convert
 * @return upscaled result of the filter
 */
int sFiltToNum(const VidFilter filt);

/**
 * @brief blitScaled Blit in with proper scaling
 * @param srcSfc
 * @param srGsRect
 * @param dstSfc
 * @param dstRect
 * @param filter
 */
void blitScaled(SDL_Surface *srcSfc,
                SDL_Rect &srGsRect,
                SDL_Surface *dstSfc,
                SDL_Rect &dstRect,
                const VidFilter filter);

#endif /* CSCALER_H_ */
