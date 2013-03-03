/*
 * CVibrate.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#include "CVibrate.h"
#include "sdl/CVideoDriver.h"

CVibrate::CVibrate(Uint32 msecs) :
m_StartTime(g_pTimer->getTicks()),
m_RunTime(msecs)
{}

// Spawns the vibration effect
void CVibrate::process()
{
	SDL_Surface *sfc = g_pVideoDriver->getBlitSurface();
	SDL_Rect gamerect = g_pVideoDriver->getGameResolution().SDLRect();
	SDL_Rect newrect;

	// First we need to figure out the coordinates
	if(m_dir_x == 0 && m_dir_y == 0) m_dir_x = -1;
	else if(m_dir_x == -1 && m_dir_y == 0) { m_dir_y = -1; m_dir_x = 0; }
	else if(m_dir_x == 0 && m_dir_y == -1) { m_dir_x = 1; m_dir_y = 0; }
	else if(m_dir_x == 1 && m_dir_y == 0) { m_dir_x = 1; m_dir_y = 1; }
	else { m_dir_x = 0; m_dir_y = 0; }

	// then erase the entire old surface ...
#if SDL_VERSION_ATLEAST(2, 0, 0)
    
#else
    mpVibSfc.reset( SDL_DisplayFormat(sfc), &SDL_FreeSurface );
#endif
	SDL_FillRect(sfc, &gamerect, SDL_MapRGB(sfc->format, 0,0,0));

	// ... and create that moved to some direction;
	newrect.x = m_dir_x;
	newrect.w = gamerect.w;
	newrect.y = m_dir_y;
	newrect.w = gamerect.h;

	// Blit it and free temp surface
	SDL_BlitSurface( mpVibSfc.get(), &gamerect,
					 g_pVideoDriver->getBlitSurface(), &newrect );

	// The developer set a time in the constructor. This effect will last for the given time.
	if(g_pTimer->getTicks() - m_StartTime >= m_RunTime)
		mFinished = true;
}
