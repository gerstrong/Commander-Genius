/*
 * CPixelate.cpp
 *
 *  Created on: 09.03.2010
 *      Author: gerstrong
 *
 *  Pixelate Effect. This effect appears, when in Keen 4
 *  you are beginning to see the main title screen.
 *  The pixelation you there, is that effect coded here!
 */

#include "CPixelate.h"
#include "../../sdl/CVideoDriver.h"

CPixelate::CPixelate() :
mp_OldSurface(NULL),
mp_NewSurface(NULL)
{
	getSnapshot();
}

// get the Snapshot of the old surface, so the the effect can be applied on it!
void CPixelate::getSnapshot()
{
	g_pVideoDriver->collectSurfaces();
	if(!mp_OldSurface) mp_OldSurface = SDL_DisplayFormat(g_pVideoDriver->BlitSurface);
}

// Effect cycle
void CPixelate::process()
{
	if(!mp_NewSurface)
	{
		// Get the a snapshot of the second surface which will be the destination.
		mp_NewSurface = SDL_DisplayFormat(g_pVideoDriver->BlitSurface);
	}

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	SDL_BlitSurface( mp_OldSurface, &gameres, g_pVideoDriver->BlitSurface, &gameres);
}

CPixelate::~CPixelate()
{
	if(mp_OldSurface) SDL_FreeSurface(mp_OldSurface);
}
