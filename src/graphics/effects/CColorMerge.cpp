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

CColorMerge::CColorMerge(Uint8 speed, SDL_Surface *firstsfc) :
m_Speed(speed),
m_Alpha(0),
mp_SourceSurface(NULL),
mp_TargetSurface(NULL)
{
	needsFirstSfc = true;
	needsSecondSfc = false;
	// The Function gets a surface, use it immediately
	if(firstsfc){
		mp_SourceSurface = SDL_DisplayFormat(firstsfc);
		needsFirstSfc = false;
	}
}

void CColorMerge::process()
{
	// The first Display must be obtained
	if(needsFirstSfc)
	{
		mp_SourceSurface = SDL_DisplayFormat(g_pVideoDriver->BlitSurface);
		needsFirstSfc = false;
	}

	if(needsSecondSfc)
	{
		mp_TargetSurface = SDL_DisplayFormat(g_pVideoDriver->BlitSurface);
		needsSecondSfc = false;
	}

	// Only process if the surfaces have content
	if( !mp_SourceSurface || !mp_TargetSurface) return;

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
