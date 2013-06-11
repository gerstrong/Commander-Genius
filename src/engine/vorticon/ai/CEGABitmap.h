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

#include "graphics/CBitmap.h"
#include "engine/vorticon/CVorticonSpriteObject.h"
#include <SDL.h>

class CEGABitmap : public CVorticonSpriteObject
{
public:
	CEGABitmap(CMap *p_Map, SDL_Surface *pSurface, CBitmap *pBitmap);
	void setPos(int px, int py);
    void render();

private:
	SDL_Surface *mp_Surface;
	CBitmap *mp_Bitmap;
};

#endif /* CEGABITMAP_H_ */
