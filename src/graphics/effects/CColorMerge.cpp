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
mp_SourceSurface(NULL),
mp_TargetSurface(NULL)
{
	getSnapshot();
}

// use this function. If you don't that, the effect won't work.
void CColorMerge::getSnapshot()
{
	g_pVideoDriver->blitScrollSurface();
	g_pVideoDriver->collectSurfaces();
	if(!mp_SourceSurface) mp_SourceSurface = SDL_DisplayFormat(g_pVideoDriver->BlitSurface);
	else if(!mp_TargetSurface) mp_TargetSurface = SDL_DisplayFormat(g_pVideoDriver->BlitSurface);
}

void CColorMerge::process()
{
	// Only process if the surfaces have content
	if( !mp_SourceSurface || !mp_TargetSurface)
	{
		getSnapshot();
		//return;
	}

	SDL_Surface *sfc = g_pVideoDriver->BlitSurface;
	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	// Process the effect
	SDL_SetAlpha(mp_TargetSurface, SDL_SRCALPHA, m_Alpha);

	SDL_BlitSurface( mp_SourceSurface, &gameres, sfc, &gameres);
	SDL_BlitSurface( mp_TargetSurface, &gameres, sfc, &gameres);

	if(m_Alpha + m_Speed > 255) m_Alpha = 255;
	else m_Alpha += m_Speed;

	if(m_Alpha == 255) m_finished = true;
}

CColorMerge::~CColorMerge()
{
	if(mp_SourceSurface) SDL_FreeSurface(mp_SourceSurface);
	if(mp_TargetSurface) SDL_FreeSurface(mp_TargetSurface);
}
