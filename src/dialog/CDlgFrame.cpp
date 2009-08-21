/*
 * CDlgFrame.cpp
 *
 *  Created on: 20.08.2009
 *      Author: gerstrong
 */

#include "CDlgFrame.h"
#include "../CGraphics.h"

CDlgFrame::CDlgFrame(int x, int y, int w, int h, Uint8 theme) {

	m_8x8tileheight = 8;
	m_8x8tilewidth = 8;

	m_x = x;
	m_y = y;
	m_w = w*m_8x8tilewidth;
	m_h = h*m_8x8tileheight;

	m_theme = theme;
}

CDlgFrame::~CDlgFrame() {
	// TODO Auto-generated destructor stub
}

void CDlgFrame::draw()
{
	if(m_theme == DLG_THEME_OLDSCHOOL)
	{
		drawOldShoolFrame();
	}
}

void CDlgFrame::drawOldShoolFrame()
{
	// However it should be blit to a memory map and then rendered every time. If not, no new effects can
	// be improved.

	// first draw the blank rect
	int i, j;
	for(j = 0 ; j < m_h - m_8x8tileheight ; j+= m_8x8tileheight )
	{
		for(i = 0 ; i < m_w - m_8x8tilewidth ; i+= m_8x8tilewidth )
			g_pGraphics->drawCharacter( m_x + i, m_y + j, 32); // 32 is a blank tile
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
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= m_8x8tileheight )
	{
		g_pGraphics->drawCharacter( m_x, m_y + j, 4); 		// 4 is one left-border
		g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + j, 5); // 5 is the right-border
	}

	// At last the corners
	g_pGraphics->drawCharacter( m_x, m_y , 1); // Upper-Left corner
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y, 3); // Upper-Right corner
	g_pGraphics->drawCharacter( m_x, m_y + m_h - m_8x8tileheight, 6); // Lower-Left corner
	g_pGraphics->drawCharacter( m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight, 8); // Lower-Right corner
}
