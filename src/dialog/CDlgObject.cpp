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
		const std::string &text, unsigned int delimiter)
{
	m_type = type;
	m_ID = ID;
	
	m_x = x;
	m_y = y;
	
	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED || m_type == DLG_OBJ_TEXT )
	{
		m_selectable = (m_type == DLG_OBJ_OPTION_TEXT) ? true : false;
		m_Option = new CDlgOptionText(text, delimiter);
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
const Uint32 fade_speed = 0xA;
void CDlgObject::render(SDL_Surface *dst, Uint8 scrollamt, bool highlight)
{
	if(m_type == DLG_OBJ_OPTION_TEXT)
	{
		// Nice colour effect
		if(m_colour < 0x0000FF && m_selected)
		{
			if( m_colour+fade_speed > 0x0000FF)
				m_colour = 0x0000FF;
			else
				m_colour+=fade_speed;
		}
		else if(m_colour > 0x000000 && !m_selected)
		{
			if( (int)(m_colour-fade_speed) < 0x000000)
				m_colour = 0x000000;
			else
				m_colour-=fade_speed;
		}

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
