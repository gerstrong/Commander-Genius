/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "COptions.h"
#include "../../sdl/CVideoDriver.h"
#include "../../sdl/CSettings.h"

COptions::COptions(char &menu_type, stOption *p_option) :
CBaseMenu(menu_type),
mp_option(p_option)
{
	int i;
	std::string buf;

	mp_Dialog = new CDialog(g_pVideoDriver->FGLayerSurface, 30, NUM_OPTIONS+4);
	mp_Dialog->setFrameTheme(DLG_THEME_OLDSCHOOL);

	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		buf = (mp_option[i].value) ? 159 : ' ';
		buf += " " + mp_option[i].name;

		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, buf);
	}

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, NUM_OPTIONS+2, "Return");
}

void COptions::processSpecific()
{
	std::string buf;

	if( m_selection != -1)
	{
		if(m_selection < NUM_OPTIONS)
		{
			mp_option[m_selection].value = !mp_option[m_selection].value;
			buf = (mp_option[m_selection].value) ? 159 : ' ';
			buf += " " + mp_option[m_selection].name;

			mp_Dialog->setObjectText(m_selection, buf);
		}
		else // Just return. Those changes are applied immediately
		{
			m_MenuType = CONFIGURE;
			m_mustclose = true;

			CSettings Settings(mp_option); // Pressed Save,
			Settings.saveGameCfg();
		}
		m_selection = -1;
	}
}

COptions::~COptions() {
	delete mp_Dialog;
}
