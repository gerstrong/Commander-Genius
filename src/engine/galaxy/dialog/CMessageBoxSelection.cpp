/*
 * CMessageBoxSelection.cpp
 *
 *  Created on: 12.06.2011
 *      Author: gerstrong
 */

#include "CMessageBoxSelection.h"
#include "sdl/CVideoDriver.h"
#include "sdl/input/CInput.h"
#include "graphics/CGfxEngine.h"

const int FONT_ID = 0;



/*CMessageBoxSelection::CMessageBoxSelection( const std::string& Text, const std::list<TextEventMatchOption> &Options ) :
CMessageBoxGalaxy(Text),
m_Options(Options),
m_selection(0)
{*/
	// Center that dialog box
	/*CFont &Font = g_pGfxEngine->getFont(FONT_ID);
	mMBRect = g_pVideoDriver->getGameResolution().SDLRect();

	mMBRect.x = m_boxrect.w/2;
	mMBRect.y = m_boxrect.h/2;

	int width = 0;

	// Get the max width of all the written lines
	for( size_t i=0 ; i<mLines.size() ; i++)
	{
		const int newwidth = Font.getPixelTextWidth(m_Lines[i]);
		if( width < newwidth )
			width = newwidth;
	}

	std::list<TextEventMatchOption>::iterator it = m_Options.begin();
	for( ; it != m_Options.end() ; it++)
	{
		const int newwidth = Font.getPixelTextWidth(it->text);
		if( width < newwidth )
			width = newwidth;
	}

	width += 16;

	m_text_height = Font.getPixelTextHeight();
	mMBRect.h = (m_text_height+2)*(m_Lines.size()+m_Options.size()+ 1) + 16;
	mMBRect.w = width;

	mMBRect.x -= m_boxrect.w/2;
	mMBRect.y -= m_boxrect.h/2;

	mp_DlgFrame = new CDlgFrame(mMBRect.x, mMBRect.y,
			mMBRect.w, mMBRect.h, DLG_THEME_GALAXY);

	mMBRect.x = 8;
	mMBRect.y = 8;*/
/*}


void CMessageBoxSelection::process()
{*/
	/*SDL_Surface *sfc = g_pVideoDriver->mp_VideoEngine->getBlitSurface();

	// Look, if somebody pressed a button, and close this dialog!
	if(g_pInput->getPressedCommand(IC_JUMP))
	{
		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

		for( int c=0 ; c<m_selection ; c++ )
			m_Options.pop_front();

		EventContainer.add( m_Options.front().event );

		m_mustclose = true;
		g_pInput->flushCommands();
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
	//g_pGfxEngine->getFont(FONT_ID).setBGColour(sfc->format, 0xFFFFFFFF);
	//g_pGfxEngine->getFont(FONT_ID).setFGColour(sfc->format, 0xFF000000);

	// Draw the Text on our surface
	for( size_t i=0 ; i<m_Lines.size() ; i++)
		g_pGfxEngine->getFont(FONT_ID).drawFont(sfc, m_Lines[i], m_boxrect.x+m_TextPos.x, m_boxrect.y+(i*m_text_height+m_TextPos.y) );


	std::list<TextEventMatchOption>::iterator it = m_Options.begin();
	for( int i=0 ; it != m_Options.end() ; it++, i++)
	{
		if(i == m_selection)
			g_pGfxEngine->getFont(FONT_ID).setFGColour(sfc->format, 0xFF0000FF);
		else
			g_pGfxEngine->getFont(FONT_ID).setFGColour(sfc->format, 0xFF000000);

		g_pGfxEngine->getFont(FONT_ID).drawFont(sfc, it->text, m_boxrect.x+m_TextPos.x, m_boxrect.y+((m_Lines.size()+i+1)*m_text_height+m_TextPos.y) );
	}*/
/*}*/

