/*
 * CFlash.cpp
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#include "CFlash.h"
#include "../../sdl/CVideoDriver.h"

CFlash::CFlash(Uint32 msecs, Uint8 speed, Uint32 color) :
m_StartTime(g_pTimer->getTicks()),
m_RunTime(msecs),
m_Speed(speed),
m_Color(color),
m_Alpha(0),
m_FadeDir(FADE_IN),
m_Style(FADE_PULSE),
m_MaxAlpha(255)
{}

CFlash::CFlash(Uint32 msecs, Uint8 speed, Uint32 color, Uint8 m_maxalpha) :
m_StartTime(g_pTimer->getTicks()),
m_RunTime(msecs),
m_Speed(speed),
m_Color(color),
m_Alpha(0),
m_FadeDir(FADE_IN),
m_Style(FADE_NORMAL),
m_MaxAlpha(m_maxalpha)
{}

// Process the flashing effect here
void CFlash::process()
{
	Uint32 ElapsedTime = g_pTimer->getTicks() - m_StartTime;
	SDL_Surface *sfc = g_pVideoDriver->BlitSurface;
	SDL_Rect gamerect = g_pVideoDriver->getGameResolution();

	// then erase the entire old surface ...
	SDL_Surface *temp = SDL_DisplayFormat(sfc);
	SDL_FillRect(temp, &gamerect, m_Color);

	SDL_SetAlpha(sfc, SDL_SRCALPHA, m_Alpha);

	// Blit it and free temp surface
	SDL_BlitSurface(temp, &gamerect, sfc, &gamerect);
	SDL_FreeSurface(temp);

	if(m_FadeDir == FADE_IN)
	{
		if(m_Alpha+m_Speed > m_MaxAlpha) m_Alpha = m_MaxAlpha;
		else m_Alpha+=m_Speed;
	}
	if(m_FadeDir == FADE_OUT)
	{
		if(m_Alpha+m_Speed < 0) m_Alpha = 0;
		else m_Alpha-=m_Speed;
	}

	if(m_Style == FADE_PULSE)
	{
		if(m_Alpha == 255) m_FadeDir = FADE_OUT;
		else if(m_Alpha == 0) m_FadeDir = FADE_IN;
	}
	else if(m_Style == FADE_NORMAL)
	{
		if(ElapsedTime >= m_RunTime/2 ) m_FadeDir = FADE_OUT;
		if(m_FadeDir == FADE_OUT && m_Alpha == 0) m_finished = true;
	}

	// The developer set a time in the constructor. This effect w ill last for the given time.
	if(ElapsedTime >= m_RunTime)
		m_finished = true;
}

CFlash::~CFlash()
{
}
