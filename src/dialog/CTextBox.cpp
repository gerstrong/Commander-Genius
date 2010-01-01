/*
 * CTextBox.cpp
 *
 *  Created on: 30.07.2009
 *      Author: gerstrong
 *
 *  TODO: Explain, what that function does.
 */

#include "CTextBox.h"
#include "../graphics/CGfxEngine.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/CTimer.h"
#include "../sdl/CInput.h"

CTextBox::CTextBox(int y, int h, const std::string& message) :
m_tw_waittime(0)
{
	setup(y, h, message);
	m_surface = g_pVideoDriver->FGLayerSurface;
}

CTextBox::CTextBox(SDL_Surface *surface, int y, int h, const std::string& message) :
m_tw_waittime(0)
{
	setup(y, h, message);
	m_surface = surface;
}

void CTextBox::setup(int y, int h, const std::string& message)
{
	int width = message.length()*8;
	if(width>280) width = 280;
	m_border = false; m_numchars = 0;
	m_timer = 0;
	m_rect.w = width+2;	m_rect.h = h;
	m_rect.x = (300/2)-(m_rect.w/2)-2;	m_rect.y = y;
	m_String = message;
	m_textdelay = 0;
	g_pInput->flushAll();
	resetTimer();
}

void CTextBox::resetTimer()
{
	m_time_start = g_pTimer->getTicks();
	m_time_passed = 0;
}

void CTextBox::setAttribs(Uint8 tw_waittime, Uint8 lettertype )
{
	m_tw_waittime = tw_waittime;
	m_lettertype = lettertype;
}

void CTextBox::process()
{
	m_time_passed = g_pTimer->getTicks() - m_time_start;
	CFont &Font = g_pGfxEngine->getFont();

	if(m_border) SDL_FillRect(m_surface, &m_rect, SDL_MapRGB(m_surface->format, 0,0,0));
	if( m_tw_waittime == 0) // means no typewritting mode!
	{
		Font.drawFont(m_surface, m_String, m_rect.x+1, m_rect.y+1, m_lettertype); 	// 0 is blank colour
	}
	else
	{
		std::string text;
		if( m_timer>=m_tw_waittime && m_numchars < m_String.size() )
		{
			m_numchars++;
			m_timer = 0;
		} else m_timer++;

		text = m_String.substr(0, m_numchars);
		Font.drawFont(m_surface, text, m_rect.x+1, m_rect.y+1, m_lettertype); 	// 0 is blank colour

		// Process Input here!
		if( g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) )
		{
			if(m_numchars != m_String.size())
				m_numchars = m_String.size();
			else
				m_textdelay = m_time_passed;
		}
	}
	// NOTE: Preloaded fonts.
	// not all colours are supported
}

bool CTextBox::hasFinished()
{
	if( m_time_passed >= m_textdelay && m_tw_waittime!=0  )	return true;

	if( m_tw_waittime==0 && g_pInput->getPressedAnyCommand() ) return true;

	return false;
}
