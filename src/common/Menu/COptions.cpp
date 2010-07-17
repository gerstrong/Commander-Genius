/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "COptions.h"
#include "../../sdl/CInput.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CSettings.h"

COptions::COptions(Uint8 dlg_theme, stOption *p_option) :
CBaseMenu(dlg_theme),
mp_option(p_option)
{
	int i;
	std::string buf;
	m_select = -1;

	mp_Dialog = new CDialog(27, NUM_OPTIONS+2, INPUT_MODE_SWITCH, dlg_theme);
	
	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, "");
		buf = mp_option[i].menuname + " ";
		buf += (mp_option[i].value) ? 28 : 20;
		buf += (mp_option[i].value) ? 29 : 21;
		buf += (mp_option[i].value) ? 30 : 22;
		buf += (mp_option[i].value) ? 31 : 23;
		mp_Dialog->m_dlgobject.at(i)->m_Option->m_FontMapID = 1;
		mp_Dialog->setObjectText(i, buf);
	}
	mp_Dialog->setSwitch(mp_option[0].value);
}

void COptions::processSpecific()
{
	std::string buf;

	if(m_mustclose) // If menu is about to close save the options
	{
		CSettings Settings(mp_option);
		Settings.saveGameCfg();
	}
	
	if( m_select != mp_Dialog->getSelection())
	{
		m_select = mp_Dialog->getSelection();
		mp_Dialog->setSwitch(mp_option[m_select].value);
	}
	else if( m_select != NO_SELECTION)
	{
		mp_option[m_select].value = mp_Dialog->getSwitch();
		buf = mp_option[m_select].menuname + " ";
		buf += (mp_option[m_select].value) ? 28 : 20;
		buf += (mp_option[m_select].value) ? 29 : 21;
		buf += (mp_option[m_select].value) ? 30 : 22;
		buf += (mp_option[m_select].value) ? 31 : 23;

		mp_Dialog->setObjectText(m_select, buf);
	}

	if( m_selection != NO_SELECTION)
	{
		mp_Dialog->setSwitch(!mp_option[m_selection].value);
		m_selection = NO_SELECTION;
	}
}

COptions::~COptions() {
	delete mp_Dialog;
}
