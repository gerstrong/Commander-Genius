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

COptions::COptions(char &menu_type, stOption *p_option) :
CBaseMenu(menu_type),
mp_option(p_option)
{
	int i;
	std::string buf;

	mp_Dialog = new CDialog(27, NUM_OPTIONS+2);
	mp_Dialog->setFrameTheme(DLG_THEME_OLDSCHOOL);

	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		buf = mp_option[i].menuname + " ";
		buf += (mp_option[i].value) ? "(On)" : "(Off)";

		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, buf);
	}
}

void COptions::processSpecific()
{
	std::string buf;
	
	if( g_pInput->getPressedCommand(IC_QUIT) )
	{
			m_MenuType = CONFIGURE;
			m_mustclose = true;

			CSettings Settings(mp_option); // Pressed Save,
			Settings.saveGameCfg();
	}

	if( m_selection != -1)
	{
			mp_option[m_selection].value = !mp_option[m_selection].value;
			buf = mp_option[m_selection].menuname + " ";
			buf += (mp_option[m_selection].value) ? "(On)" : "(Off)";

			mp_Dialog->setObjectText(m_selection, buf);
			m_selection = -1;
	}
}

COptions::~COptions() {
	delete mp_Dialog;
}
