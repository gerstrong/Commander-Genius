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

	m_line = 0;
	m_lines_completed = 0;
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

	// This is the algorithm for drawing the effect. It was invented by myself, so no guarantee,
	// it will look the same as in Keen Galaxy. We will see, when it's finished
	// for(unsigned short y=m_lines_completed ; y<m_line ; y++)
	// {
		// while(pixel is already painted there)
		// {
			// choose another pixel in the same line
		// }

		// The c line gets two pixels painted at random x, between 0 and 320.

		// m_pixels_per_line[y]++;

		// If there are no more pixels to draw in this line, m_lines_completed++, it won't be scanned again.
		// This will be checked against m_pixels_per_line

	// }
	// m_line++;
	// if( m_lines_completed >= m_line )
		// In this case the algorithm has finished
		// m_finished = true;

	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	SDL_BlitSurface( mp_OldSurface, &gameres, g_pVideoDriver->BlitSurface, &gameres);
}

CPixelate::~CPixelate()
{
	if(mp_OldSurface) SDL_FreeSurface(mp_OldSurface);
}
