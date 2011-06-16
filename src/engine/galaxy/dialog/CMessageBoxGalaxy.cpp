/*
 * CMessageBoxGalaxy.cpp
 *
 *  Created on: 30.03.2011
 *      Author: gerstrong
 *
 *  This
 */

#include "CMessageBoxGalaxy.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "graphics/CGfxEngine.h"

const int FONT_ID = 0;

CMessageBoxGalaxy::CMessageBoxGalaxy(const std::string& Text) :
CMessageBox(Text, false, false, false)
{
	// Center that dialog box
	CFont &Font = g_pGfxEngine->getFont(FONT_ID);
	m_boxrect = g_pVideoDriver->getGameResolution();

	m_boxrect.x = m_boxrect.w/2;
	m_boxrect.y = m_boxrect.h/2;

	int width = 0;
	for( size_t i=0 ; i<m_Lines.size() ; i++)
	{
		const int newwidth = Font.getPixelTextWidth(m_Lines[i]);
		if( width < newwidth )
			width = newwidth;
	}

	width += 16;

	m_text_height = Font.getPixelTextHeight();
	m_boxrect.h = (m_text_height+2)*m_Lines.size()+16;
	m_boxrect.w = width;

	m_boxrect.x -= m_boxrect.w/2;
	m_boxrect.y -= m_boxrect.h/2;

	mp_DlgFrame = new CDlgFrame(m_boxrect.x, m_boxrect.y,
			m_boxrect.w, m_boxrect.h, DLG_THEME_GALAXY);

	m_TextPos.x = 8;
	m_TextPos.y = 8;
}

void CMessageBoxGalaxy::process()
{
	SDL_Surface *sfc = g_pVideoDriver->mp_VideoEngine->getFGLayerSurface();

	// Look, if somebody pressed a button, and close this dialog!
	if(g_pInput->getPressedAnyCommand()) {
		m_mustclose = true;
		return;
	}

	// Draw the empty Dialog Box
	mp_DlgFrame->draw(sfc);

	// Set the proper Font colors
	g_pGfxEngine->getFont(FONT_ID).setBGColour(sfc->format, 0xFFFFFFFF);
	g_pGfxEngine->getFont(FONT_ID).setFGColour(sfc->format, 0xFF000000);

	// Draw the Text on our surface
	for( size_t i=0 ; i<m_Lines.size() ; i++)
		g_pGfxEngine->getFont(FONT_ID).drawFont(sfc, m_Lines[i], m_boxrect.x+m_TextPos.x, m_boxrect.y+(i*m_text_height+m_TextPos.y) );
}
