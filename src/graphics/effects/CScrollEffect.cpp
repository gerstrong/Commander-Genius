/*
 * CScrollEffect.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CScrollEffect.h"
#include "sdl/CVideoDriver.h"

CScrollEffect::CScrollEffect(SDL_Surface *pScrollSurface, SDL_Surface *pBackground, Sint8 speed) :
m_Speed(speed),
m_ScrollPos(0),
mp_OldSurface(NULL),
mp_ScrollSurface(pScrollSurface)
{
	mp_OldSurface = SDL_DisplayFormat(pBackground);

	 if(m_Speed < 0)
		 m_ScrollPos = mp_ScrollSurface->h;
}

void CScrollEffect::process()
{
	SDL_Rect gameres = g_pVideoDriver->getGameResolution();
	SDL_Rect dest = gameres;
	SDL_Rect src = gameres;

	src.y = mp_ScrollSurface->h-m_ScrollPos;
	dest.h = m_ScrollPos;

	//SDL_BlitSurface( mp_OldSurface, &gameres, g_pVideoDriver->getBlitSurface(), &gameres);
	SDL_BlitSurface( mp_ScrollSurface, &src, g_pVideoDriver->getBlitSurface(), &dest);


	if(m_Speed < 0)
	{
		if(m_ScrollPos + m_Speed < 0) m_ScrollPos = 0;
		else m_ScrollPos += m_Speed;

		if(m_ScrollPos == 0) m_finished = true;
	}
	else
	{
		if(m_ScrollPos + m_Speed > mp_OldSurface->h) m_ScrollPos = mp_ScrollSurface->h;
		else m_ScrollPos += m_Speed;

		if(m_ScrollPos == mp_ScrollSurface->h) m_finished = true;
	}
}

CScrollEffect::~CScrollEffect() {
	SDL_Rect gameres = g_pVideoDriver->getGameResolution();

	// So the final image is loaded correctly
	//SDL_BlitSurface( mp_OldSurface, &gameres, g_pVideoDriver->getBlitSurface(), &gameres);
	if(m_Speed > 0)
		SDL_BlitSurface( mp_ScrollSurface, &gameres, g_pVideoDriver->getBlitSurface(), &gameres);

	if(mp_OldSurface) SDL_FreeSurface(mp_OldSurface);
}
