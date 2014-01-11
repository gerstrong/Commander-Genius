/*
 * CDlgFrame.cpp
 *
 *  Created on: 20.08.2009
 *      Author: gerstrong
 */

#include "CDlgFrame.h"
#include "graphics/GsGraphics.h"

CDlgFrame::CDlgFrame(int x, int y, int w, int h, int tilewidth, int tileheight)
{
	
	m_8x8tileheight = tilewidth;
	m_8x8tilewidth = tileheight;
	
	m_x = x;
	m_y = y;
	resize(w, h);
	
	m_theme = g_pBehaviorEngine->getEngine();
}


int CDlgFrame::TileWidth()
{
	return m_8x8tilewidth;
}


void CDlgFrame::draw(SDL_Surface *dst)
{
	if( m_theme == ENGINE_VORTICON )
	{
		drawVorticonFrame(dst);
    }
	else if(m_theme == ENGINE_GALAXY )
	{
		//drawGalaxyFrame(dst);
	}
}



void CDlgFrame::resize(const Uint32 width, const Uint32 height)
{
	m_w = width; m_h = height;
}




void CDlgFrame::setPos(const Uint32 x, const Uint32 y)
{
	m_x = x ; m_y = y;
}




void CDlgFrame::drawVorticonFrame(SDL_Surface *dst)
{
	GsFont &Font = gGraphics.getFont(1);

	// first draw the blank rect
	int i, j;

	SDL_FillRect(dst, NULL, 0xFFFFFFFF);

	for(j = 0 ; j < m_h - m_8x8tileheight ; j+= m_8x8tileheight )
		Font.drawCharacter(dst, 32, m_w - m_8x8tilewidth, m_y + j); // for the last tile

	// then the borders
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
	{
		Font.drawCharacter(dst, 2,  i, 0); // 2 is one upper-border
		Font.drawCharacter(dst, 7,  i, m_h - m_8x8tileheight); // 7 is also the lower-border
	}
	Font.drawCharacter(dst, 2,  m_w - m_8x8tilewidth, 0); // for the last tile
	Font.drawCharacter(dst, 2,  m_w - m_8x8tilewidth, m_h - m_8x8tileheight); // for the last tile
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= m_8x8tileheight )
	{
		Font.drawCharacter(dst, 4, 0, j ); // 4 is one left-border
		Font.drawCharacter(dst, 5, 0 + m_w - m_8x8tilewidth, j ); // 5 is the right-border
	}

	// At last the corners
	Font.drawCharacter(dst, 1, 0, 0 ); // Upper-Left corner
	Font.drawCharacter(dst, 3, m_w - m_8x8tilewidth, 0 ); // Upper-Right corner
	Font.drawCharacter(dst, 6, 0, m_h - m_8x8tileheight ); // Lower-Left corner
	Font.drawCharacter(dst, 8, m_w - m_8x8tilewidth, m_h - m_8x8tileheight ); // Lower-Right corner
}




