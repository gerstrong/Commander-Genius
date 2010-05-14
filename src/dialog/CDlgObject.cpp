/*
 * CDlgObject.cpp
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 *
 *  An Object of the dialog controls that can be selected and maybe manipulated
 */

#include "CDlgObject.h"
#include "../graphics/CFont.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/video/colourconvert.h"

CDlgObject::CDlgObject() :
m_Option(NULL),
m_selectable(false),
m_selected(false),
m_colour(0x0)
{ }

///
// Creation Routine
///
void CDlgObject::create(Uint8 type, Uint16 ID, Uint16 x, Uint16 y,
		const std::string &text, unsigned int delimiter, Uint8 Fontmap_ID, Uint8 theme)
{
	m_type = type;
	m_ID = ID;
	m_theme = theme;
	
	m_x = x;
	m_y = y;
	
	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED || m_type == DLG_OBJ_TEXT )
	{
		m_selectable = (m_type == DLG_OBJ_OPTION_TEXT) ? true : false;
		m_Option = new CDlgOptionText(text, delimiter, Fontmap_ID);
	}
}

void CDlgObject::change(unsigned int delimiter, const std::string &text, Uint8 type)
{
	m_type = type;
	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED)
	{
		m_selectable = (m_type == DLG_OBJ_OPTION_TEXT) ? true : false;
		m_Option->setText(text, delimiter);
	}
	else if( m_type == DLG_OBJ_TEXT )
	{
		m_selectable = false;
		m_Option->setText(text, delimiter+4);
	}
}

///
// Property change routine
///
// Will this item get selected or not?
void CDlgObject::setSelection(bool value)
{
	m_selected = value;
}

///
// Rendering Routine
///
const Uint8 fade_speed = 0xA;
void CDlgObject::render(SDL_Surface *dst, Uint8 scrollamt, bool highlight)
{
	if(m_type == DLG_OBJ_OPTION_TEXT)
	{
		Uint32 colourdir;

		if(m_theme == DLG_THEME_GALAXY)
			colourdir = m_selected ? 0x54fc54 : 0x208c20;
		else
			colourdir = m_selected ? 0x0000FF : 0x000000;

		if(g_pVideoDriver->getSpecialFXConfig())	// Nice colour effect applied here
			fadetoColour(m_colour,colourdir, fade_speed);
		else
			m_colour = colourdir;

		m_Option->draw(dst, m_x+16, m_y-8*scrollamt, highlight, m_colour);
	}
	else if(m_type == DLG_OBJ_DISABLED)
		m_Option->draw(dst, m_x+16, m_y-8*scrollamt, highlight, 0x7F7F7F); // +16 because selection icon needs space
	else if(m_type == DLG_OBJ_TEXT)
		m_Option->draw(dst, m_x, m_y-8*scrollamt, true);
}

///
// Erase Routine
///
CDlgObject::~CDlgObject(){
	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED || m_type == DLG_OBJ_TEXT )
	{
		if(m_Option) delete m_Option;
	}
}
