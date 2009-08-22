/*
 * CTextViewer.cpp
 *
 *  Created on: 12.08.2009
 *      Author: gerstrong
 */

#include "../keen.h"

#include "../sdl/CInput.h"
#include "../include/gamedo.h"
#include "CTextViewer.h"
#include "../CGraphics.h"

CTextViewer::CTextViewer(int x, int y, int w, int h) {
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;

	m_linepos = 0;
	m_scrollpos = 0;

	m_8x8tileheight = 8;
	m_8x8tilewidth = 8;
}

void CTextViewer::scrollDown()
{
	if( m_scrollpos < m_8x8tileheight )
		m_scrollpos++;
	else
	{
		if( m_linepos < (int)m_textline.size() - (m_h/m_8x8tileheight) )
		{
			m_scrollpos = 0;
			setNextPos();
		}
	}
}

void CTextViewer::scrollUp()
{
	if( m_scrollpos > 0 )
		m_scrollpos--;
	else
	{
		if( m_linepos > 0 )
		{
			m_scrollpos = m_8x8tileheight;
			setPrevPos();
		}
	}
}

void CTextViewer::setNextPos()
{	setPosition(m_linepos+1);	}

void CTextViewer::setPrevPos()
{	setPosition(m_linepos-1);	}

void CTextViewer::setPosition(int pos)
{	if( pos>=0 && pos < (int) m_textline.size() - (m_h/m_8x8tileheight-1) )
		m_linepos = pos;	}

void CTextViewer::loadText(const std::string text)
{
	mp_text = text;

	// Trim strings the way they fit into the textbox
	std::string buf;
	int totlen=0;
	for(unsigned long i=0 ; i<mp_text.size() ; i++ )
	{
		buf.push_back(mp_text[i]);

		if( mp_text[i] == '\n' )
		{
			m_textline.push_back(buf);
			buf.clear();
			continue;
		}
		if(mp_text[i+1] == 26)
		{
			mp_text[i+1] = '\n';
			mp_text[i+2] = ' ';
			mp_text[i+3] = ' ';
		}

		totlen=buf.size() + getnextwordlength(mp_text.c_str()+i+1);
		if( totlen > (m_w/m_8x8tilewidth-2) && mp_text[i] != '_' ) // Or does the next fit into the line?
		{
			m_textline.push_back(buf);
			buf.clear();
		}
	}

	// Afterworks: First, the last line has a delimiter of 26. That really sucks! The rest after that must be trimmed
	//			   Second, we add two empty lines for a nice style when the text is scrolled.
	m_textline.push_back(""); // Append two empty lines for a nice style, when scrolling
	m_textline.push_back("");

	// Change the colours to read and grey background, when '~' is detected at the beginning
	// Also remove the escape sequences
	std::vector<std::string>::iterator it;
	for( it = m_textline.begin() ; it != m_textline.end()  ; it++  )
	{
		for(unsigned int j=0 ; j < it->size() ; j++ )
		{
			if( (*it)[j] == 31 ||
				(*it)[j] ==	13 ||
				(*it)[j] == 10 )  (*it)[j] = ' ';
		}

		if( (*it)[0] == '~' )
		{
			for(unsigned int j=1 ; j < it->size() ; j++ )
			{
				(*it)[j] += 128;        // Magic number that makes the text red with grey background
				(*it)[j-1] = (*it)[j];  // Move all the character one unit to the left
			}

			while( it->size() < (unsigned int)(m_w/m_8x8tilewidth) ) // if the text is smaller than the width of the textbox, fill it with grey background
				it->push_back(' ' + 128);
		}
	}
}

unsigned char CTextViewer::getnextwordlength(const std::string nextword)
{
	int i=0;
	while(nextword[i] != ' ' && nextword[i] != '\n' && nextword[i] != 26 ) i++;
	return i;
}

void CTextViewer::drawTextlines()
{
	for(int i=1 ; i<(m_h/m_8x8tileheight) && i<(int)m_textline.size()-m_linepos ; i++)
		g_pGraphics->drawFont(m_textline[i+m_linepos-1],
							  m_x+m_8x8tilewidth,
							  m_y + (i)*m_8x8tileheight-m_scrollpos,
							  false);
}

// Most common render function for this TextViewer
void CTextViewer::processCycle()
{
	char timer=0;
	bool cancel = false;

	do
	{
		gamedo_fades();
		gamedo_AnimatedTiles();

		g_pInput->pollEvents();

		// Normal Keys/Axes
		if( g_pInput->getHoldedCommand(IC_DOWN) )
		{
			timer++;
			if(timer >= 5)
				scrollDown();
		}
		if( g_pInput->getHoldedCommand(IC_UP) )
		{
			timer++;
			if(timer >= 5)
				scrollUp();
		}

		// Page Keys
		if( g_pInput->getHoldedKey(KPGDN) )
		{
			timer++;
			if(timer >= 5)
				setNextPos();
		}
		if( g_pInput->getHoldedKey(KPGUP) )
		{
			timer++;
			if(timer >= 5)
				setPrevPos();
		}

		if(timer>=10) timer=0;

		cancel = g_pInput->getPressedKey(KQUIT);

		gamedo_RenderScreen();
		renderBox(); // This comes after, because it does transparent overlay
	} while(!cancel);
}

// This function shows the Story of Commander Keen!
void CTextViewer::renderBox()
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

	if(!m_textline.empty())
		drawTextlines();

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
	g_pGraphics->drawFont("ESC to Exit / \17 \23 to Read",
						m_x+m_8x8tilewidth+(m_w/2)-12*m_8x8tilewidth, m_y+m_h, true);
}
