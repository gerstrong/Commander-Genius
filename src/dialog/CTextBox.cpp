/*
 * CTextBox.cpp
 *
 *  Created on: 30.07.2009
 *      Author: gerstrong
 */

#include "CTextBox.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"

CTextBox::CTextBox(int x, int y, const std::string& message){
	m_rect.x = x;	m_rect.y = y;
	m_rect.w = 120;	m_rect.h = 24;
	m_String = message;
	m_surface = g_pVideoDriver->FGLayerSurface;
}

CTextBox::CTextBox(SDL_Surface *surface, int x, int y, const std::string& message){
	m_rect.x = x;	m_rect.y = y;
	m_rect.w = 100;	m_rect.h = 24;
	m_String = message;
	m_surface = surface;
}

void CTextBox::process()
{
	SDL_FillRect(m_surface, &m_rect, SDL_MapRGB(m_surface->format, 255,255,255));
	g_pGfxEngine->Font->drawFont(m_surface, m_String, m_rect.x+8, m_rect.y+8, 0); 	// 0 is blank colour
																		// NOTE: Preloaded fonts.
																		// not all colours are supported
}
