/*
 * CDialog.cpp
 *
 *  Created on: 20.03.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "../include/gamedo.h"
#include "../sdl/CTimer.h"
#include "../sdl/CInput.h"
#include "../sdl/CVideoDriver.h"
#include "../CGraphics.h"
#include "CDialog.h"

CDialog::CDialog(Uint16 x, Uint16 y, Uint16 w, Uint16 h)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;

	m_twirl.posy = y;
	m_twirl.frame = 0;
	m_twirl.timer = 0;

	m_selected_ID = 0;
	m_scroll = 0;

	m_Frame = NULL;
}

CDialog::~CDialog(){

	for(Uint8 i=0 ; i<m_dlgobject.size() ; i++ )
	{
		delete m_dlgobject[i]; // the first vector element must be cleared
	}

	while(!m_dlgobject.empty())
		m_dlgobject.pop_back();

	if(m_Frame) delete m_Frame;
}

///
// Creation Routines for Objects
///
void CDialog::setFrameTheme(Uint8 theme)
{
	m_Frame = new CDlgFrame(m_x, m_y, m_w, m_h, theme);
}

void CDialog::addObject(Uint8 type, Uint16 x, Uint16 y,const std::string text)
{
	CDlgObject *DlgObject = new CDlgObject();
	DlgObject->create(type, m_dlgobject.size(), x, y, text, m_w-((x-m_x)/8)-4 );
	m_dlgobject.push_back(DlgObject);

	for(Uint8 i=0 ; i<m_dlgobject.size() ; i++) // go the next selectable item
	{
		if(!m_dlgobject[m_selected_ID]->m_selectable)
		{
			m_selected_ID++;
		}
	}
}

void CDialog::setObjectText(Uint8 ID, const std::string &text)
{
	if( m_dlgobject[ID]->m_type == DLG_OBJ_OPTION_TEXT )
	{
		m_dlgobject[ID]->change(text, m_w-((m_dlgobject[ID]->m_x-m_x)/8)-4);
	}
}

///
// Retrieval functions
///
int CDialog::getSelection()
{	return m_selected_ID;
}

///
// Rendering routine
///

void CDialog::processlogic()
{
	if(g_pInput->getPulsedCommand(IC_DOWN, 80))
	{
		do
		{
			if(m_selected_ID == m_dlgobject.size()-1)
			{
				m_selected_ID = 0;
				m_scroll=0;
			}
			else
			{
				m_selected_ID++;

				if(m_selected_ID >= m_h-2+m_scroll)
					m_scroll++;
			}

		} while(!m_dlgobject[m_selected_ID]->m_selectable);
	}
	else if(g_pInput->getPulsedCommand(IC_UP, 80))
	{
		do
		{
			if(m_selected_ID == 0)
			{
				m_selected_ID = m_dlgobject.size()-1;

				if(m_dlgobject.size() > m_h)
					m_scroll = m_selected_ID-(m_h-2)+1;
			}
			else
			{
				m_selected_ID--;

				if(m_selected_ID < m_scroll)
					m_scroll--;
			}
		} while(!m_dlgobject[m_selected_ID]->m_selectable);
	}

	g_pInput->pollEvents();
}

void CDialog::render()
{
	if( g_pTimer->TimeToRender() == false ) return;

	// do fades
	gamedo_fades();

	// Render the empty Dialog frame if any
	if(m_Frame) m_Frame->draw();

	// Draw the to icon up or down accordingly
	if( m_scroll>0 ) // Up Arrow
	{
		g_pGraphics->drawCharacter(m_Frame->m_x+m_Frame->m_w-16,m_Frame->m_y+8,15);
	}
	if( ( m_h-2 < (Uint8) m_dlgobject.size() )  &&
		( m_scroll != m_dlgobject.size()-m_h+2 )) // Down Arrow
	{
		g_pGraphics->drawCharacter(m_Frame->m_x+m_Frame->m_w-16,
									m_Frame->m_y+m_Frame->m_h-16 ,19);
	}

	Uint8 max;
	if( m_h+m_scroll-2 > (Uint8)m_dlgobject.size() )
		max = m_dlgobject.size();
	else
		max = m_h+m_scroll-2;

	// they are processed by ID of course
	for(Uint16 i=m_scroll ;	i<max ; i++)
	{
		m_dlgobject[i]->render(m_scroll, (i==m_selected_ID) );
	}

	// Render the twirl
	renderTwirl();

	// blit the scrollbuffer to the display
	g_pVideoDriver->sb_blit();
	g_pTimer->TimeToDelay();
}

#define TWIRL_TIME	10
#define TWIRL_SPEED	10
void CDialog::renderTwirl()
{
	if( m_twirl.timer >= TWIRL_TIME )
	{
		m_twirl.timer = 0;

		if(m_twirl.frame >= 5)
			m_twirl.frame=0;
		else
			m_twirl.frame++;
	}
	else m_twirl.timer++;

	if(m_dlgobject[m_selected_ID]->m_y < m_twirl.posy)
		m_twirl.posy--;
	else if(m_dlgobject[m_selected_ID]->m_y > m_twirl.posy)
		m_twirl.posy++;

	g_pGraphics->drawCharacter(m_dlgobject[m_selected_ID]->m_x,
								m_twirl.posy, 9+m_twirl.frame);
}

///
// Misc Routines
///

