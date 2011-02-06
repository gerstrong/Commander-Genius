/*
 * CColorMerge.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 *
 *
 */

#include "CColorMerge.h"
#include "../../sdl/CVideoDriver.h"

CColorMerge::CColorMerge(Uint8 speed) :
m_Speed(speed),
m_Alpha(0),
mp_OldSurface(NULL)
{
	getSnapshot();
}

// use this function. If you don't that, the effect won't work.
void CColorMerge::getSnapshot()
{
	g_pVideoDriver->collectSurfaces();
	if(!mp_OldSurface) mp_OldSurface = SDL_DisplayFormat(g_pVideoDriver->mp_VideoEngine->getBlitSurface());
}

// Effect cycle
void CColorMerge::process()
{
	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	// Process the effect
	SDL_SetAlpha( mp_OldSurface, SDL_SRCALPHA, 255-m_Alpha );

	SDL_BlitSurface( mp_OldSurface, &gameres, g_pVideoDriver->getBlitSurface(), &gameres);

	if(m_Alpha + m_Speed > 255) m_Alpha = 255;
	else m_Alpha += m_Speed;

	if(m_Alpha == 255) m_finished = true;
}

CColorMerge::~CColorMerge()
{
	if(mp_OldSurface) SDL_FreeSurface(mp_OldSurface);
}
