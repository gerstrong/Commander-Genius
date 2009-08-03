/*
 * CTextBox.cpp
 *
 *  Created on: 30.07.2009
 *      Author: gerstrong
 */

#include "CTextBox.h"
#include "../CGraphics.h"
#include "../StringUtils.h"

CTextBox::CTextBox(float x, float y, float w, float h, const std::string& text, bool border_rel) {
	m_fontwidth = 0;
	m_fontheight = 0;
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
	m_String = text;
	m_border_relative = border_rel;
}

void CTextBox::setFontDimensions(float width, float height)
{
	m_fontwidth = width;
	m_fontheight = (height>0.0f) ? height : width; // 0 it is square
	if(m_border_relative)
	{
		m_x += m_fontwidth;
		m_y += m_fontheight;
		m_w -= 2*m_fontwidth;
		m_h -= 2*m_fontheight;
	}
}

void CTextBox::format()
{
	// This function format properly the text into a box (multiple lines),
	// separating the strings by words.

	std::string word;
	std::string::iterator it = m_String.begin();
	std::string oneline;
	unsigned int i = 0;

	while(m_String.length() > i)
	{
		word = GetNextWord(it, m_String);

		if((oneline.length() + word.length()) > m_w/m_fontwidth) // if the line is full
		{
			if(*(oneline.end()-1) == ' ') oneline.erase(oneline.end()-1);
			m_Textline.push_back(oneline);
			oneline.clear();
		}
		oneline += word + " ";

		it += word.length() + 1;
		i += word.length() + 1;
	}
	// The last line might not be full, but must also be printed
	m_Textline.push_back(oneline);

}

int CTextBox::getNumberOfTextlines()
{
	return m_Textline.size();
}

void CTextBox::draw()
{
	// Draw Routine for the Textbox
	for(unsigned int i=0 ; i<m_Textline.size() ; i++ )
	{
		g_pGraphics->drawFont(m_Textline[i], m_x, m_y+i*m_fontheight, 0); // 0 is blank colour
		if( i*m_fontheight > m_h )
			break;
	}
}
