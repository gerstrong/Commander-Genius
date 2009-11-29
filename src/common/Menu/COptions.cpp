/*
 * COptions.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "COptions.h"

COptions::COptions(char &menu_type) :
CBaseMenu(menu_type) {
	//mp_Dialog = new CDialog(mp_MenuSurface, 22, NUM_OPTIONS+5);
	/*int i;
	std::string buf;

	for( i = 0 ; i < NUM_OPTIONS ; i++ )
	{
		buf = mp_option[i].name + ": ";

		if(mp_option[i].value)
			buf += "Enabled";
		else
			buf += "Disabled";

		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i+1, buf);
	}

	mp_Dialog->addObject(DLG_OBJ_TEXT, 1, NUM_OPTIONS+2, "");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, NUM_OPTIONS+3, "Return");*/
}

void COptions::processSpecific()
{
	std::string buf;
/*
	if( m_selection != -1)
	{
		if(m_selection < NUM_OPTIONS)
		{
			buf = mp_option[m_selection].name + ": ";

			if(mp_option[m_selection].value)
			{
				mp_option[m_selection].value = 0;
				buf += "Disabled";
			}
			else
			{
				mp_option[m_selection].value = 1;
				buf += "Enabled";
			}

			mp_Dialog->setObjectText(m_selection, buf);
			mp_Dialog->setObjectText(NUM_OPTIONS, "Save and Return");
			//mp_Dialog->setObjectType(NUM_OPTIONS, DLG_OBJ_DISABLED);
			mp_Dialog->setObjectText(NUM_OPTIONS+1, "Cancel");
		}
		else
		{
			if(m_selection == NUM_OPTIONS)
			{
				CSettings Settings(mp_option); // Pressed Save,
				Settings.saveGameCfg();
			}
			m_goback = true;
		}
		m_selection = -1;
	}

	if(m_goback)
	{
		cleanup();
		init(CONFIGURE);
	}*/

}

COptions::~COptions() {
	//delete mp_Dialog;
}
