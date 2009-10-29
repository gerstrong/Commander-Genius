/*
 * CTextBox.cpp
 *
 *  Created on: 30.07.2009
 *      Author: gerstrong
 */

#include "CTextBox.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"

CTextBox::CTextBox(int y, const std::string& message){
	m_rect.w = message.length()*8+2;	m_rect.h = 10;
	m_rect.x = (300/2)-(m_rect.w/2)+10;	m_rect.y = y;
	m_String = message;
	m_surface = g_pVideoDriver->FGLayerSurface;
}

CTextBox::CTextBox(SDL_Surface *surface, int y, const std::string& message){
	m_rect.w = message.length()*8+2;	m_rect.h = 10;
	m_rect.x = (300/2)-(m_rect.w/2);	m_rect.y = y;
	m_String = message;
	m_surface = surface;
}

void CTextBox::process()
{
	SDL_FillRect(m_surface, &m_rect, SDL_MapRGB(m_surface->format, 0,0,0));
	g_pGfxEngine->Font->drawFont(m_surface, m_String, m_rect.x+1, m_rect.y+1, 1); 	// 0 is blank colour
																		// NOTE: Preloaded fonts.
																		// not all colours are supported
}
