/*
 * CMessageBoxSelection.cpp
 *
 *  Created on: 12.06.2011
 *      Author: gerstrong
 */

#include "CMessageBoxSelection.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CInput.h"
#include "graphics/CGfxEngine.h"

const int FONT_ID = 0;

CMessageBoxSelection::CMessageBoxSelection( const std::string& Text, const std::list<TextEventMatchOption> &Options ) :
CMessageBoxGalaxy(Text),
m_Options(Options),
m_selection(0)
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


void CMessageBoxSelection::process()
{
	SDL_Surface *sfc = g_pVideoDriver->mp_VideoEngine->getFGLayerSurface();

	// Look, if somebody pressed a button, and close this dialog!
	if(g_pInput->getPressedCommand(IC_JUMP))
	{
		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

		for( int c=0 ; c<m_selection ; c++ )
			m_Options.pop_front();

		EventContainer.add( m_Options.front().event );

		m_mustclose = true;
		return;
	}
	else if(g_pInput->getPressedCommand(IC_DOWN))
	{
		if(m_selection >= (m_Options.size()-1) )
			m_selection = 0;
		else
			m_selection++;
	}
	else if(g_pInput->getPressedCommand(IC_UP))
	{
		if(m_selection <= 0 )
			m_selection = m_Options.size()-1;
		else
			m_selection--;
	}


	// Draw the empty Dialog Box
	mp_DlgFrame->draw(sfc);

	// Set the proper Font colors
	g_pGfxEngine->getFont(FONT_ID).setBGColour(sfc->format, 0xFFFFFFFF);
	g_pGfxEngine->getFont(FONT_ID).setFGColour(sfc->format, 0xFF000000);

	// Draw the Text on our surface
	for( size_t i=0 ; i<m_Lines.size() ; i++)
		g_pGfxEngine->getFont(FONT_ID).drawFont(sfc, m_Lines[i], m_boxrect.x+m_TextPos.x, m_boxrect.y+(i*m_text_height+m_TextPos.y) );

	for( size_t i=0 ; i<m_Options.size() ; i++)
		g_pGfxEngine->getFont(FONT_ID).drawFont(sfc, m_Options.back().text, m_boxrect.x+m_TextPos.x, m_boxrect.y+((m_Lines.size()+i+1)*m_text_height+m_TextPos.y) );

}

