/*
 * CWindow.cpp
 *
 *  Created on: 29.07.2009
 *      Author: gerstrong
 */

#include "CWindow.h"
#include "../CGraphics.h"

CWindow::CWindow(float x, float y, float w, float h, char window_type)
{
	// TODO: Change that back to uint input as all the entire class. With Float it is a nightmare here!
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;

	m_window_type = window_type;

	m_8x8tileheight = 8.0/200.0;
	m_8x8tilewidth = 8.0/320.0;
}

CWindow::~CWindow() {
	// Empty list ID, while also emptying
	int textbox_ctr = 0;
	while(!m_ID_List.empty())
	{
		switch(*m_ID_List.begin())
		{
		case OBJ_TYPE_TEXT:
			delete m_TextBox[textbox_ctr++];
			break;
		}
		m_ID_List.pop_front();
	}
}

///////////////////////
// Creation Routines //
///////////////////////
void CWindow::addObject(CTextBox* newTextBox)
{
	//float x, float y, float w, float h, const std::string& text, bool border_rel;
	// Element coordinates are all relative and must go inside the borders if desired!
	// if false it will be relative to the window itself and can be used as title border

	m_TextBox.push_back(newTextBox);
	m_TextBox.back()->format();
	m_ID_List.push_back(OBJ_TYPE_TEXT);
}

///////////////////////////
// Property Set Routines //
///////////////////////////
void CWindow::Resize(float width, float height)
{
	m_w = width;
	m_h = height;
}

///////////////////////////
// Property Get Routines //
///////////////////////////
float CWindow::getWidth()
{
	return m_w;
}

////////////////////////
// Rendering Routines //
////////////////////////
void CWindow::render()
{
	// draw the window
	drawWindow();

	// Draw the window objects which are inside
	// Process the ID-List and render every Element of the Window
	std::list<int> :: iterator i;
	int textbox_ctr = 0;
	for( i = m_ID_List.begin() ; i != m_ID_List.end() ; i++ )
	{
		switch(*i)
		{
		case OBJ_TYPE_TEXT:
			m_TextBox[textbox_ctr++]->draw();
			break;
		}
	}
}

void CWindow::drawWindow()
{
	// If we use floating values, the drawing routine is very important to get a well sized window.

	// However it should be blit to a memory map and then rendered every time. If not, no new effects can
	// be improved.

	// first draw the blank rect
	float i, j;
	for(j = 0.0 ; j < m_h - m_8x8tileheight ; j+= (m_8x8tileheight*(7.0/8.0)) )
	{
		for(i = 0.0 ; i < m_w - m_8x8tilewidth ; i+= m_8x8tilewidth )
			g_pGraphics->drawCharacter( m_x + i, m_y + j, 32); // 32 is blank tile
		g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + j, 32); // for the last tile
	}

	// then the borders
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
	{
		g_pGraphics->drawCharacter( m_x + i, m_y, 2); 							// 2 is one upper-border
		g_pGraphics->drawCharacter( m_x + i, m_y + m_h - m_8x8tileheight, 7); 	// 7 is also the lower-border
	}
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y, 2);	// for the last tile
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight, 2); // for the last tile
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= (m_8x8tileheight*(7.0/8.0)) )
	{
		g_pGraphics->drawCharacter( m_x, m_y + j, 4); 		// 4 is one left-border
		g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + j, 5); // 5 is the right-border
	}

	// At last the corners
	g_pGraphics->drawCharacter( m_x, m_y , 1); // Upper-Left corner
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y, 3); // Upper-Right corner
	g_pGraphics->drawCharacter( m_x, m_y + m_h - m_8x8tileheight, 6); // Lower-Left corner
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight, 8); // Lower-Right corner

	if(m_window_type == WND_PAGESCROLLER)
	{
		// It has Scroll Controls, so the user knows, that he can scroll down, up and quit
		// fill the area with grey tiles
		for( i=m_8x8tilewidth ; i<m_w-m_8x8tilewidth ; i+=m_8x8tilewidth )
			for( j=0 ; j<2*m_8x8tileheight ; j+=m_8x8tileheight )
				g_pGraphics->drawCharacter( m_x+i, m_y+m_h+j, 160);	// just grey small tile

		g_pGraphics->drawCharacter( m_x, m_y + m_h, 4); 						// 4 is one left-border
		g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + m_h, 5); 	// 5 is the right-border
		for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
			g_pGraphics->drawCharacter( m_x + i, m_y + m_h + m_8x8tileheight, 7); 					// 7 is also the lower-border
		g_pGraphics->drawCharacter( m_x, m_y + m_h + m_8x8tileheight, 6); 						// Lower-Left corner
		g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + m_h + m_8x8tileheight, 8); 	// Lower-Right corner

		// Now print the helping text
		g_pGraphics->drawFont("ESC to Exit / \17 \21 to Read",
							m_x+m_8x8tilewidth+(m_w/2)-12*m_8x8tilewidth
								, m_y+m_h, true);
	}
}
