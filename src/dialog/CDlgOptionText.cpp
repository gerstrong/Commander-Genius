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

CDlgOptionText::~CDlgOptionText() {
}

void CDlgOptionText::draw(SDL_Surface *Textsurface, Uint16 x, Uint16 y, bool highlight)
{
	g_pGfxEngine->Font.drawFont(Textsurface, m_text, x, y, highlight);
}
