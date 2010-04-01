/*
 * CDlgOptionText.cpp
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#include "CDlgOptionText.h"

#include "../graphics/CGfxEngine.h"

CDlgOptionText::CDlgOptionText(const std::string &text, Uint8 delimiter, Uint8 text_type) :
m_text("")
{
	setText(text, delimiter);
	m_type = text_type;
	m_selectable = !(m_type == DISABLED);
}

void CDlgOptionText::setText(const std::string &text, Uint8 delimiter) {
	m_text = text;
	
	if(m_text.size() > delimiter && delimiter > 0 )
	{
		m_text.erase( delimiter );
		m_text += "...";
	}
}

void CDlgOptionText::render(SDL_Surface *dst, Uint8 scrollamt, Uint8 highlight)
{
	if(m_type == NORMAL)
		draw(dst, m_x+16, m_y-8*scrollamt, highlight); // +16 because selection icon needs space
	else if(m_type == DISABLED)
		draw(dst, m_x+16, m_y-8*scrollamt, LETTER_TYPE_DISABLED);
}

void CDlgOptionText::processInput()
{
	m_previtem = m_nextitem = false;

	if(g_pInput->getPressedCommand(IC_UP))
	{
		m_previtem = true;
	}
	else if(g_pInput->getPressedCommand(IC_DOWN))
	{
		m_nextitem = true;
	}
}

void CDlgOptionText::draw(SDL_Surface *Textsurface, Uint16 x, Uint16 y, Uint8 highlight)
{
	g_pGfxEngine->getFont().drawFont(Textsurface, m_text, x, y, highlight);
}

CDlgOptionText::~CDlgOptionText() {
}


