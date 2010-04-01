/*
 * CDlgObject.cpp
 *
 *  Created on: 19.08.2009
 *      Author: gerstrong
 */

#include "CDlgObject.h"
#include "../graphics/CFont.h"

CDlgObject::CDlgObject() :
m_selectable(true),
m_selected(false),
m_previtem(false),
m_nextitem(false)
//m_Option(NULL)
{
}

///
// Creation Routine
///

/*void CDlgObject::create(Uint8 type, Uint16 ID, Uint16 x, Uint16 y, const std::string &text, unsigned int delimiter)
{
	m_type = type;
	m_ID = ID;
	
	m_x = x;
	m_y = y;
	
	
	if( m_type == DLG_OBJ_OPTION_TEXT || m_type == DLG_OBJ_DISABLED || m_type == DLG_OBJ_TEXT )
	{
		m_selectable = (m_type == DLG_OBJ_OPTION_TEXT) ? true : false;
		//m_Option = new CDlgOptionText(text, delimiter);
	}
}*/

// The new create DlgObject function. Used as generic pattern
void CDlgObject::create(Uint16 ID, Uint16 x, Uint16 y)
{
	m_ID = ID;
	m_x = x;
	m_y = y;
}

/*void CDlgObject::change(unsigned int delimiter, const std::string &text, Uint8 type)
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
}*/

void CDlgObject::looseFocus()
{
	m_previtem = m_nextitem = m_selected = false;
}

bool CDlgObject::setFocus()
{
	if(m_selectable)
	{
		m_selected = true;
		return true;
	}
	else
	{
		return false;
	}
}

bool CDlgObject::getPrevItem()
{	return	m_previtem;	}

bool CDlgObject::getNextItem()
{	return	m_nextitem;	}

///
// Property change routine
///

///
// Rendering Routine for basic neutral objects
///
void CDlgObject::render(SDL_Surface *dst, Uint8 scrollamt, Uint8 highlight)
{
	/*if(m_type == DLG_OBJ_OPTION_TEXT)
		m_Option->draw(dst, m_x+16, m_y-8*scrollamt, highlight); // +16 because selection icon needs space
	else if(m_type == DLG_OBJ_DISABLED)
		m_Option->draw(dst, m_x+16, m_y-8*scrollamt, LETTER_TYPE_DISABLED); // +16 because selection icon needs space
	else if(m_type == DLG_OBJ_TEXT)
		m_Option->draw(dst, m_x, m_y-8*scrollamt, LETTER_TYPE_RED);*/
}

///
// Destruction Routine
///
CDlgObject::~CDlgObject(){
	//if(m_Option) delete m_Option;
}

