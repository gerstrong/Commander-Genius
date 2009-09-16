/*
 * CDlgObject.cpp
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#include "CDlgObject.h"
#include "../graphics/CFont.h"

CDlgObject::CDlgObject(){
	m_selectable = false;
	m_OptionText = NULL;
}

///
// Erase Routine
///
CDlgObject::~CDlgObject(){
	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED )
	{
		if(m_OptionText) delete m_OptionText;
	}
}

///
// Creation Routine
///

void CDlgObject::create(Uint8 type, Uint16 ID, Uint16 x, Uint16 y, const std::string &text, unsigned int delimiter)
{
	m_type = type;
	m_ID = ID;

	m_x = x;
	m_y = y;


	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED )
	{
		m_selectable = (m_type == DLG_OBJ_OPTION_TEXT) ? true : false;
		m_OptionText = new CDlgOptionText(text, delimiter);
	}
}

void CDlgObject::change(const std::string &text, unsigned int delimiter)
{
	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED )
	{
		m_selectable = true;
		m_OptionText->setText(text, delimiter);
	}
}

///
// Property change routine
///

///
// Rendering Routine
///
void CDlgObject::render(SDL_Surface *dst, Uint8 scrollamt, Uint8 highlight)
{
	if(m_type == DLG_OBJ_OPTION_TEXT)
		m_OptionText->draw(dst, m_x+16, m_y-8*scrollamt, highlight); // +16 because selection icon needs space
	else if(m_type == DLG_OBJ_DISABLED)
		m_OptionText->draw(dst, m_x+16, m_y-8*scrollamt, LETTER_TYPE_DISABLED); // +16 because selection icon needs space
}
