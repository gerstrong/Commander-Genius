/*
 * CDlgOptionText.cpp
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#include "CDlgOptionText.h"

#include "../graphics/CGfxEngine.h"

CDlgOptionText::CDlgOptionText(const std::string &text, unsigned int delimit) {
	setText(text, delimit);
}

void CDlgOptionText::setText(const std::string &text, unsigned int delimit) {
	m_text = text;
	
	if(m_text.size() > delimit )
	{
		m_text.erase(delimit-3);
		m_text += "...";
	}
}

void CDlgOptionText::draw(SDL_Surface *Textsurface, Uint16 x, Uint16 y, Uint8 highlight)
{
	g_pGfxEngine->getFont().drawFont(Textsurface, m_text, x, y, highlight);
}

CDlgOptionText::~CDlgOptionText() {
}


