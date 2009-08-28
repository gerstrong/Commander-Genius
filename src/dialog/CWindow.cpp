/*
 * CWindow.cpp
 *
 *  Created on: 29.07.2009
 *      Author: gerstrong
 */

#include "CWindow.h"
#include "../graphics/CGfxEngine.h"

CWindow::CWindow(SDL_Surface *Surface, int x, int y, int w, int h)
{
	m_x = x;	m_y = y;
	m_w = w;	m_h = h;

	m_8x8tileheight = m_8x8tilewidth = 8;

	m_Surface = Surface;
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
void CWindow::Resize(int width, int height)
{
	m_w = width;
	m_h = height;
}

///////////////////////////
// Property Get Routines //
///////////////////////////
int CWindow::getWidth()
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
	// first draw the blank rect
	int i, j;
	for(j = 0 ; j < m_h - m_8x8tileheight ; j+= m_8x8tileheight )
	{
		for(i = 0 ; i < m_w - m_8x8tilewidth ; i+= m_8x8tilewidth )
			g_pGfxEngine->Font.drawCharacter( m_Surface, 32, m_x + i, m_y + j); // 32 is blank tile
		g_pGfxEngine->Font.drawCharacter( m_Surface, 32, m_x + m_w - m_8x8tilewidth, m_y + j); // for the last tile
	}
	// then the borders
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
	{
		g_pGfxEngine->Font.drawCharacter( m_Surface, 2, m_x + i, m_y); 							// 2 is one upper-border
		g_pGfxEngine->Font.drawCharacter( m_Surface, 7, m_x + i, m_y + m_h - m_8x8tileheight); 	// 7 is also the lower-border
	}
	g_pGfxEngine->Font.drawCharacter( m_Surface, 2, m_x + m_w - m_8x8tilewidth, m_y);	// for the last tile
	g_pGfxEngine->Font.drawCharacter( m_Surface, 2, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight); // for the last tile
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= m_8x8tileheight )
	{
		g_pGfxEngine->Font.drawCharacter( m_Surface, 4, m_x, m_y + j); 		// 4 is one left-border
		g_pGfxEngine->Font.drawCharacter( m_Surface, 5, m_x + m_w - m_8x8tilewidth, m_y + j); // 5 is the right-border
	}
	// At last the corners
	g_pGfxEngine->Font.drawCharacter( m_Surface, 1, m_x, m_y ); // Upper-Left corner
	g_pGfxEngine->Font.drawCharacter( m_Surface, 3, m_x + m_w - m_8x8tilewidth, m_y); // Upper-Right corner
	g_pGfxEngine->Font.drawCharacter( m_Surface, 6, m_x, m_y + m_h - m_8x8tileheight); // Lower-Left corner
	g_pGfxEngine->Font.drawCharacter( m_Surface, 8,m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight); // Lower-Right corner
}
