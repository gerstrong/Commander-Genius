/*
 * CWindow.cpp
 *
 *  Created on: 29.07.2009
 *      Author: gerstrong
 */

#include "CWindow.h"

#include "../CGraphics.h"

CWindow::CWindow(float x, float y, float w, float h)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;

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
	// Use we use floating values, the drawing routine is very important to get a well sized window.

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
		g_pGraphics->drawCharacter( m_x + i, m_y, 2); 		// 2 is one upper-border
		g_pGraphics->drawCharacter( m_x + i, m_y + m_h - m_8x8tileheight, 7); // 2 is also the lower-border
	}
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y, 2);	// for the last tile
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight, 2); // for the last tile
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= (m_8x8tileheight*(7.0/8.0)) )
	{
		g_pGraphics->drawCharacter( m_x, m_y + j, 4); 		// 4 is one left-border
		g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + j, 5); // 4 is also the right-border
	}

	// At last the corners
	g_pGraphics->drawCharacter( m_x, m_y , 1); // Upper-Left corner
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y, 3); // Upper-Right corner
	g_pGraphics->drawCharacter( m_x, m_y + m_h - m_8x8tileheight, 6); // Lower-Left corner
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight, 8); // Lower-Right corner
}
