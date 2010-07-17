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

	mp_Dialog = new CDialog(27, NUM_OPTIONS+2, INPUT_MODE_UP_DOWN, dlg_theme);
	
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
}

void COptions::processSpecific()
{
	std::string buf;

	if(m_mustclose) // If menu is about to close save the options
	{
		CSettings Settings(mp_option);
		Settings.saveGameCfg();
	}

	if( m_selection != NO_SELECTION)
	{
		mp_option[m_selection].value = !mp_option[m_selection].value;
		buf = mp_option[m_selection].menuname + " ";
		buf += (mp_option[m_selection].value) ? 28 : 20;
		buf += (mp_option[m_selection].value) ? 29 : 21;
		buf += (mp_option[m_selection].value) ? 30 : 22;
		buf += (mp_option[m_selection].value) ? 31 : 23;

		mp_Dialog->setObjectText(m_selection, buf);
		m_selection = NO_SELECTION;
	}
}

COptions::~COptions() {
	delete mp_Dialog;
}
