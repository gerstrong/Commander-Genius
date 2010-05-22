/*
 * CSettingsMenu.cpp
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#include "CSettingsMenu.h"
#include "CVideoSettings.h"
#include "CAudioSettings.h"
#include "COptions.h"
#include "CControlsettings.h"
#include "CControlsettings.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CSettingsMenu::CSettingsMenu( Uint8 dlgtheme, stOption *p_option ) :
CBaseMenu(dlgtheme),
mp_SubMenu(NULL),
mp_option(p_option)
{
	mp_Dialog = new CDialog(13, 6, INPUT_MODE_UP_DOWN,m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Graphics");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Audio");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Options");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Controls");
}

void CSettingsMenu::processSpecific()
{
	if(!mp_SubMenu)
	{
		if( m_selection != NO_SELECTION)
		{
			switch(m_selection)
			{
			case 0: mp_SubMenu = new CVideoSettings(m_dlg_theme); break;
			case 1: mp_SubMenu = new CAudioSettings(m_dlg_theme); break;
			case 2: mp_SubMenu = new COptions(m_dlg_theme, mp_option); break;
			case 3: mp_SubMenu = new CControlsettings(m_dlg_theme); break;
			}

			m_selection = NO_SELECTION;
			m_suspended = true;
		}
	}
	else
	{
		mp_SubMenu->processCommon();
		mp_SubMenu->processSpecific();
		mp_SubMenu->postProcess();

		if(mp_SubMenu->mustClose())
		{
			SAFE_DELETE(mp_SubMenu);
			m_suspended = false;
		}
	}
}

CSettingsMenu::~CSettingsMenu()
{
	// TODO Auto-generated destructor stub
}
