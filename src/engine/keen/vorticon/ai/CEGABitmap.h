/*
 * CEGABitmap.h
 *
 *  Created on: 23.10.2009
 *      Author: gerstrong
 *
 *  Only used for showing Bitmaps
 */

#ifndef CEGABITMAP_H_
#define CEGABITMAP_H_

#include "graphics/GsBitmap.h"
#include "../CVorticonSpriteObject.h"
#include <SDL.h>

class CEGABitmap : public CVorticonSpriteObject
{
public:
	CEGABitmap(CMap *p_Map, SDL_Surface *pSurface, GsBitmap *pBitmap);
	void setPos(int px, int py);
    void draw();

private:
	SDL_Surface *mp_Surface;
	GsBitmap *mp_Bitmap;
};

#endif /* CEGABITMAP_H_ */
