/*
 * CTextViewer.cpp
 *
 *  Created on: 12.08.2009
 *      Author: gerstrong
 */

#include "../keen.h"

#include "../sdl/CInput.h"
#include "CTextViewer.h"
#include "../graphics/CGfxEngine.h"

CTextViewer::CTextViewer(SDL_Surface *TextVSfc, int x, int y, int w, int h) {
	m_x = x;	m_y = y;
	m_w = w;	m_h = h;
	
	m_scrollpos = m_linepos = 0;
	m_8x8tilewidth = m_8x8tileheight = 8;
	m_TextVSfc = TextVSfc;
	m_mustclose = false;	m_timer = 0;
}

void CTextViewer::scrollDown()
{
	if( m_scrollpos < m_8x8tileheight )
	{
		m_scrollpos+=4;
	}
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
	{
		m_scrollpos-=4;
	}
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

void CTextViewer::loadText(const std::string &text)
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
		g_pGfxEngine->Font->drawFont(m_TextVSfc,
									 m_textline[i+m_linepos-1],
									 m_x+m_8x8tilewidth,
									 m_y + (i)*m_8x8tileheight-m_scrollpos,
									 false);
}

// Most common render function for this TextViewer
void CTextViewer::process()
{
	 // Normal Keys/Axes
	 if( g_pInput->getHoldedCommand(IC_DOWN) )
	 {
		 m_timer++;
		 if(m_timer >= 2)
			 scrollDown();
	 }
	 if( g_pInput->getHoldedCommand(IC_UP) )
	 {
		 m_timer++;
		 if(m_timer >= 2)
			 scrollUp();
	 }
	 
	 // Page Keys
	 if( g_pInput->getPressedKey(KPGDN) )
		 setPosition(m_linepos+16);
	 if( g_pInput->getPressedKey(KPGUP) )
		 setPosition(m_linepos-16);
	 
	 if(m_timer>=8) m_timer=0;
	 
	 if(g_pInput->getPressedKey(KQUIT) || g_pInput->getPressedKey(KQ) )
		 m_mustclose = true;
	 
	 renderBox(); // This comes after, because it does transparent overlay
}

// This function shows the Story of Commander Keen!
void CTextViewer::renderBox()
{
	// first draw the blank rect
	int i, j;
	for(j = 0 ; j < m_h - m_8x8tileheight ; j+= m_8x8tileheight )
	{
		for(i = 0 ; i < m_w - m_8x8tilewidth ; i+= m_8x8tilewidth )
			g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 32, m_x + i, m_y + j); // 32 is a blank tile
		g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 32, m_x + m_w - m_8x8tilewidth, m_y + j);
	}
	
	if(!m_textline.empty())
		drawTextlines();
	
	// then the borders
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
	{
		g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 2, m_x + i, m_y);	// 2 is one upper-border
		g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 7, m_x + i, m_y + m_h - m_8x8tileheight );  // 7 is also the lower-border
	}
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 2, m_x + m_w - m_8x8tilewidth, m_y );	// for the last tile
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 2, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight );	// for the last tile
	
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= m_8x8tileheight )
	{
		g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 4, m_x, m_y + j); 		// 4 is one left-border
		g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 5, m_x + m_w - m_8x8tilewidth, m_y + j); // 5 is the right-border
	}
	
	// At last the corners
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 1, m_x, m_y ); // Upper-Left corner
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 3, m_x + m_w - m_8x8tilewidth, m_y ); // Upper-Right corner
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 6, m_x, m_y + m_h - m_8x8tileheight ); // Lower-Left corner
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 8, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight ); // Lower-Right corner
	
	// It has Scroll Controls, so the user knows, that he can scroll down, up and quit
	// fill the area with grey tiles
	for( i=m_8x8tilewidth ; i<m_w-m_8x8tilewidth ; i+=m_8x8tilewidth )
		for( j=0 ; j<2*m_8x8tileheight ; j+=m_8x8tileheight )
			g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 160, m_x+i, m_y+m_h+j );	// just grey small tile
	
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 4, m_x, m_y + m_h ); 						// 4 is one left-border
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 5, m_x + m_w - m_8x8tilewidth, m_y + m_h ); 	// 5 is the right-border
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
		g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 7, m_x + i, m_y + m_h + m_8x8tileheight ); 					// 7 is also the lower-border
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 6, m_x, m_y + m_h + m_8x8tileheight ); 						// Lower-Left corner
	g_pGfxEngine->Font->drawCharacter(m_TextVSfc, 8, m_x + m_w - m_8x8tilewidth, m_y + m_h + m_8x8tileheight ); 	// Lower-Right corner
	
	// Now print the helping text
	g_pGfxEngine->Font->drawFont(m_TextVSfc, "ESC to Exit / \17 \23 to Read",
								 m_x+m_8x8tilewidth+(m_w/2)-12*m_8x8tilewidth, m_y+m_h, true);
}
