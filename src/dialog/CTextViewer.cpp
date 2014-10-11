/*
 * CTextViewer.cpp
 *
 *  Created on: 12.08.2009
 *      Author: gerstrong
 */

#include <fstream>

#include <base/CInput.h>
#include "CTextViewer.h"
#include "graphics/GsGraphics.h"
#include "sdl/extensions.h"
#include <base/video/CVideoDriver.h>
#include <base/utils/FindFile.h>
#include <base/GsLogging.h>

CTextViewer::CTextViewer(int x, int y, int w, int h) :
m_timer(0)
{
	m_x = x;	m_y = y;
	m_w = w;	m_h = h;
	
	m_scrollpos = m_linepos = 0;
	m_8x8tilewidth = m_8x8tileheight = 8;
	m_mustclose = false;

	SDL_Surface *temp = CG_CreateRGBSurface( gVideoDriver.getGameResolution().SDLRect() );

    mpTextVSfc.reset(gVideoDriver.convertThroughBlitSfc(temp), &SDL_FreeSurface);

    SDL_FreeSurface(temp);
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
{
    if( pos>=0 && pos < (int) m_textline.size() - (m_h/m_8x8tileheight-1) )
        m_linepos = pos;
}

/**
 * \brief 	This function formats the text the way it's correctly shown in the
 * \param	text	The text that already has been read but is not yet correctly formated
 * 					This happen, because the Keen text files are a bit different...
 */
void CTextViewer::formatText(const std::string &text)
{
	mp_text = text;
	
	// Trim strings the way they fit into the textbox
	// TODO: Use more C++ specific stuff and less code to perform that trimming operations
	std::string buf;
	int totlen=0;
	for(unsigned long i=0 ; i<mp_text.size() ; i++ )
	{
		buf.push_back(mp_text[i]);
		
		if( mp_text[i] == '\n' )
		{
            if(!buf.empty())
                buf = buf.substr(0, buf.size()-1);

			m_textline.push_back(buf);
			buf.clear();
			continue;
		}

		if(i+3 < mp_text.size())
		{
			if(mp_text[i+1] == 26)
			{
				mp_text[i+1] = '\n';
				mp_text[i+2] = ' ';
				mp_text[i+3] = ' ';
			}
		}
		
		if(i+1 < mp_text.size())
			totlen=buf.size() + getnextwordlength(mp_text.c_str()+i+1);
		else
			totlen=buf.size() + getnextwordlength(mp_text.c_str()+i);

        if( totlen > (m_w/m_8x8tilewidth-2) && mp_text[i] != '_' ) // Or does the next one fit into the line?
		{
            if(!buf.empty())
                buf = buf.substr(0, buf.size()-1);

			m_textline.push_back(buf);
			buf = "";
		}
	}
	
	// Afterworks: First, the last line has a delimiter of 26. That really sucks! The rest after that must be trimmed
	//			   Second, we add two empty lines for a nice style when the text is scrolled.
	// Append an empty line for a nice style, when scrolling
	if(buf.size()>=1)
		buf.erase(buf.size()-1);
	m_textline.push_back(buf);
	m_textline.push_back("");
	
	// Change the colours to read and grey background, when '~' is detected at the beginning
	// Also remove the escape sequences
    for( auto &textitem : m_textline  )
	{
        for(unsigned int j=0 ; j < textitem.size() ; j++ )
		{
            if(textitem[j] == 31 || textitem[j] ==	13 || textitem[j] == 10 )
                textitem[j] = ' ';
		}
		
        if( textitem[0] == '~' )
		{
            for(unsigned int j=1 ; j < textitem.size() ; j++ )
			{
                textitem[j] += 128;        // Magic number that makes the text red with grey background
                textitem[j-1] = textitem[j];  // Move all the character one unit to the left
			}
			
            while( textitem.size() < (unsigned int)((m_w-1)/m_8x8tilewidth) ) // if the text is smaller than the width of the textbox, fill it with grey background
                textitem.push_back(' ' + 128);
		}
	}
}
/**
 * \brief This will load for you a text from a file into memory and of course automatically format it for you :-)
 * \parm	filename	filename to open
 */
bool CTextViewer::loadTextfromFile(const std::string &filename)
{
	std::string text;
    std::ifstream endfile;

    OpenGameFileR(endfile, filename);
    if (endfile.is_open())
    {
        while(!endfile.eof())
        {
        	text.push_back(endfile.get());
        }
        endfile.close();
        text.push_back('\0');
    }
    else
    {
    	gLogging.textOut("Error reading \"" + filename + "\". Check if this file is in your directory!");
    	return false;
    }

    formatText(text);

    return true;
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
    {
		gGraphics.getFont(1).drawFont(mpTextVSfc.get(),
									 m_textline[i+m_linepos-1],
                                     m_x+m_8x8tilewidth,
									 m_y + (i)*m_8x8tileheight-m_scrollpos,
									 false);
    }
}

// Most common render function for this TextViewer
void CTextViewer::ponder(const float deltaT)
{
	 // Normal Keys/Axes
	 if( gInput.getHoldedCommand(IC_DOWN) )
	 {
		 m_timer++;
		 if(m_timer >= 2)
			 scrollDown();
	 }
	 if( gInput.getHoldedCommand(IC_UP) )
	 {
		 m_timer++;
		 if(m_timer >= 2)
			 scrollUp();
	 }
	 
	 // Page Keys
	 if( gInput.getPressedKey(KPGDN) )
		 setPosition(m_linepos+16);
	 if( gInput.getPressedKey(KPGUP) )
		 setPosition(m_linepos-16);
	 
	 if(m_timer>=8) m_timer=0;
	 
	 if(gInput.getPressedKey(KQUIT) || gInput.getPressedKey(KQ) || gInput.getPressedCommand(IC_BACK) )
		 m_mustclose = true;	 
}



void CTextViewer::render()
{
    renderBox();
    // This comes after, because it does use semi-transparent overlay

    BlitSurface(mpTextVSfc.get(), NULL, gVideoDriver.getBlitSurface(), NULL);
}



// This function shows the Story of Commander Keen!
void CTextViewer::renderBox()
{
	SDL_Surface *sfc = mpTextVSfc.get();

	// first draw the blank rect
	GsFont &Font = gGraphics.getFont(1);
	int i, j;
	for(j = 0 ; j < m_h - m_8x8tileheight ; j+= m_8x8tileheight )
	{
		for(i = 0 ; i < m_w - m_8x8tilewidth ; i+= m_8x8tilewidth )
			Font.drawCharacter(sfc, 32, m_x + i, m_y + j); // 32 is a blank tile
		Font.drawCharacter(sfc, 32, m_x + m_w - m_8x8tilewidth, m_y + j);
	}
	
	if(!m_textline.empty())
		drawTextlines();
	
	// then the borders
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
	{
		Font.drawCharacter(sfc, 2, m_x + i, m_y);	// 2 is one upper-border
		Font.drawCharacter(sfc, 7, m_x + i, m_y + m_h - m_8x8tileheight );  // 7 is also the lower-border
	}
	Font.drawCharacter(sfc, 2, m_x + m_w - m_8x8tilewidth, m_y );	// for the last tile
	Font.drawCharacter(sfc, 2, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight );	// for the last tile
	
	for( j = m_8x8tileheight ; j < m_h-m_8x8tileheight ; j+= m_8x8tileheight )
	{
		Font.drawCharacter(sfc, 4, m_x, m_y + j); 		// 4 is one left-border
		Font.drawCharacter(sfc, 5, m_x + m_w - m_8x8tilewidth, m_y + j); // 5 is the right-border
	}
	
	// At last the corners
	Font.drawCharacter(sfc, 1, m_x, m_y ); // Upper-Left corner
	Font.drawCharacter(sfc, 3, m_x + m_w - m_8x8tilewidth, m_y ); // Upper-Right corner
	Font.drawCharacter(sfc, 6, m_x, m_y + m_h - m_8x8tileheight ); // Lower-Left corner
	Font.drawCharacter(sfc, 8, m_x + m_w - m_8x8tilewidth, m_y + m_h - m_8x8tileheight ); // Lower-Right corner
	
	// It has Scroll Controls, so the user knows, that he can scroll down, up and quit
	// fill the area with grey tiles
	for( i=m_8x8tilewidth ; i<m_w-m_8x8tilewidth ; i+=m_8x8tilewidth )
		for( j=0 ; j<2*m_8x8tileheight ; j+=m_8x8tileheight )
			Font.drawCharacter(sfc, 160, m_x+i, m_y+m_h+j );	// just grey small tile
	
	Font.drawCharacter(sfc, 4, m_x, m_y + m_h ); 						// 4 is one left-border
	Font.drawCharacter(sfc, 5, m_x + m_w - m_8x8tilewidth, m_y + m_h ); 	// 5 is the right-border
	for( i = m_8x8tilewidth ; i < m_w-m_8x8tilewidth ; i+= m_8x8tilewidth )
		Font.drawCharacter(sfc, 7, m_x + i, m_y + m_h + m_8x8tileheight ); 					// 7 is also the lower-border
	Font.drawCharacter(sfc, 6, m_x, m_y + m_h + m_8x8tileheight ); 						// Lower-Left corner
	Font.drawCharacter(sfc, 8, m_x + m_w - m_8x8tilewidth, m_y + m_h + m_8x8tileheight ); 	// Lower-Right corner
	
	// Now print the helping text
	Font.drawFont(sfc, "BACK to Exit / \17 \23 to Read",
								 m_x+m_8x8tilewidth+(m_w/2)-12*m_8x8tilewidth, m_y+m_h, true);
}
