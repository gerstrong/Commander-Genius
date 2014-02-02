/*
 * CDlgOptionText.cpp
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#include "CDlgOptionText.h"

#include <graphics/GsGraphics.h>

/**
 * \brief  	Constructor of the class. It represents a text which can be selected in the menu.
 * 			As it is an option text, it is there all the time, it can be selected, truncated, and even
 * 			have another fontmap, like the extra tile ones which can be assigned at any time. Those
 * 			are used for the sliders as an example...
 *
 * \param	text   		The text that will be written in that option item
 * \param	delimtier   Usually the width-2 of the menu box, as this will
 * 						automatically truncate the text making it fit into the box
 * \param	fontmapID	This is the ID to the fontmap which can be changed, so for example
 * 						the extra tiles in vorticons engine (for sliders) can be used
 *
 */
CDlgOptionText::CDlgOptionText(const std::string &text, unsigned int delimit, Uint8 fontmapID)
{
	setText(text, delimit);
	m_FontMapID = fontmapID;
}

/**
 * \brief  	technically it does the same as the contructor, because the constructor calls that function.
 * 			if you call it anyway, it will apply the changes you are putting into...
 *
 * \param	text   		The text that will be written in that option item
 * \param	delimtier   Usually the width-2 of the menu box, as this will
 * 						automatically truncate the text making it fit into the box
 * \param	fontmapID	This is the ID to the fontmap which can be changed, so for example
 * 						the extra tiles in vorticons engine (for sliders) can be used
 */
void CDlgOptionText::setText(const std::string &text, unsigned int delimit) {
	m_text = text;
	
	if(m_text.size() > delimit )
	{
		m_text.erase(delimit-3);
		m_text += "...";
	}
}

/**
 * \brief  	This normally is part of a routine cycle and draws the text on the SDL-Surface
 *
 * \param	Textsurface	Pointer to the SDL-Surface where Text has to be drawn.
 * 						Normally it's a blit surface
 * \param	x   		X-Coordinate where to draw the text
 * \param	y   		Y-Coordinate where to draw the text
 * 						automatically truncate the text making it fit into the box
 * \param	highlight	The vorticon engine especially has the black text and the red-grayed-one
 * 						in the same fontmap. To enable that red-grayed text, just put this parameter
 * 						to true. (normally false)
 * \param	colour		Not given it will put it into black. Since the new GFX-Engine Update you can
 * 						use any text colour you want, the fontmap will be tinted temporary to get it printed
 * 						using the colour of your wish. Caution: Changing the colour too many times in a cycle
 * 						may cause the system to slow down. If you want to write something where every letter has
 * 						a different colour, please don't use this...
 */
void CDlgOptionText::draw(SDL_Surface *Textsurface, Uint16 x, Uint16 y, bool highlight, Uint32 colour)
{
	GsFont &Font = gGraphics.getFont(m_FontMapID);

	//Font.setFGColour(Textsurface->format, colour);

	Font.drawFont(Textsurface, m_text, x, y, highlight);
}

CDlgOptionText::~CDlgOptionText()
{}


