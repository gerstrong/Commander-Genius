/*
 * CEGABitmap.cpp
 *
 *  Created on: 23.10.2009
 *      Author: gerstrong
 */

#include "CEGABitmap.h"

///
// Initialization Routine
///
CEGABitmap::CEGABitmap(CMap *p_Map, SDL_Surface *pSurface, CBitmap *pBitmap) :
CVorticonSpriteObject(p_Map,0,0, OBJ_NONE)
{
	mp_Surface = pSurface;
	mp_Bitmap = pBitmap;
}

///
// Process Routine
///
void CEGABitmap::process()
{
	mp_Bitmap->draw( scrx, scry );
}
