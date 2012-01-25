/*
 * CScrollEffect.cpp
 *
 *  Created on: 13.03.2011
 *      Author: gerstrong
 */

#include "CScrollEffect.h"
#include "sdl/CVideoDriver.h"

CScrollEffect::CScrollEffect(SDL_Surface *pScrollSurface, SDL_Surface *pBackground,
							const Sint16 initialPos, Sint8 speed) :
m_Speed(speed),
m_ScrollPos(initialPos),
mp_OldSurface(NULL),
mp_ScrollSurface(pScrollSurface)
{
	mp_OldSurface = SDL_DisplayFormat(pBackground);
}

void CScrollEffect::process()
{
	SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();
	SDL_Rect dest = gameres;
	SDL_Rect src = gameres;

	src.y = mp_ScrollSurface->h-m_ScrollPos;
	dest.h = m_ScrollPos;

	g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mp_ScrollSurface, &src,  &dest ) );


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

Sint16 CScrollEffect::getScrollPosition()
{
	return m_ScrollPos;
}

CScrollEffect::~CScrollEffect()
{
	SDL_Rect gameres = g_pVideoDriver->getGameResolution().SDLRect();

	// So the final image is loaded correctly
	if(m_Speed > 0)
		g_pVideoDriver->mDrawTasks.add( new BlitSurfaceTask( mp_ScrollSurface, &gameres,  &gameres ) );

	if(mp_OldSurface) SDL_FreeSurface(mp_OldSurface);
}
