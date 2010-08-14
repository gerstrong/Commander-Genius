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
#include "CSelectionMenu.h"
#include "sdl/CTimer.h"
#include "sdl/CVideoDriver.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CSettingsMenu::CSettingsMenu( Uint8 dlgtheme, CExeFile &ExeFile, stOption *p_option, bool &restartVideo ) :
CBaseMenu(dlgtheme),
m_ExeFile(ExeFile),
mp_SubMenu(NULL),
mp_option(p_option),
m_restartVideo(restartVideo),
m_profileselection(0)
{
	mp_Dialog = new CDialog(13, 7, INPUT_MODE_UP_DOWN,m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Graphics");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Audio");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Options");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Controls");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Profile");
}

void CSettingsMenu::processSpecific()
{
	if(!mp_SubMenu)
	{
		if( m_selection != NO_SELECTION)
		{
			switch(m_selection)
			{
			case 0: mp_SubMenu = new CVideoSettings(m_dlg_theme, m_restartVideo); break;
			case 1: mp_SubMenu = new CAudioSettings(m_dlg_theme, m_ExeFile); break;
			case 2: mp_SubMenu = new COptions(m_dlg_theme, mp_option); break;
			case 3: mp_SubMenu = new CControlsettings(m_dlg_theme); break;
			case 4:
			{
				std::list<std::string> profile_list;
				profile_list.push_back("Classic mode (like DOS)");
				profile_list.push_back("Enhanced mode (more stuff)");

				mp_SubMenu = new CSelectionMenu<Uint8>( m_profileselection, profile_list, m_dlg_theme );
			}
			break;
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

			if(m_profileselection == 1)
				setDefaultClassic();
			else if(m_profileselection == 2)
				setDefaultEnhanced();
		}
	}
}
/**
 * \brief This sets the default settings for a classic gameplay
 */
void CSettingsMenu::setDefaultClassic()
{
	m_profileselection = 0;

	g_pVideoDriver->setSpecialFXMode(false);
	st_camera_bounds &CameraBounds = g_pVideoDriver->getCameraBounds();
	CameraBounds.left = 160;
	CameraBounds.up = 100;
	CameraBounds.right = 161;
	CameraBounds.down = 101;
	CameraBounds.speed = 10;
	g_pTimer->setFrameRate(DEFAULT_LPS, 30, DEFAULT_SYNC);

	mp_option[OPT_SUPERPOGO].value = 0;
	mp_option[OPT_ALLOWPKING].value = 0;
	mp_option[OPT_KEYSTACK].value = 0;
	mp_option[OPT_ANALOGJOYSTICK].value = 0;
	mp_option[OPT_LVLREPLAYABILITY].value = 0;
	mp_option[OPT_RISEBONUS].value = 0;
	mp_option[OPT_IMPPOGO].value = 0;
	mp_option[OPT_HUD].value = 0;

	CSettings Settings;
	Settings.saveDrvCfg();

	// And close this menu...
	m_restartVideo = true;
}

/**
 * \brief This sets the default settings for an enhanced gameplay
 */
void CSettingsMenu::setDefaultEnhanced()
{
	m_profileselection = 0;

	g_pVideoDriver->setSpecialFXMode(true);
	st_camera_bounds &CameraBounds = g_pVideoDriver->getCameraBounds();
	CameraBounds.left = 152;
	CameraBounds.up = 92;
	CameraBounds.right = 168;
	CameraBounds.down = 108;
	CameraBounds.speed = 20;
	g_pTimer->setFrameRate(DEFAULT_LPS, 60, DEFAULT_SYNC);

	mp_option[OPT_KEYSTACK].value = 1;
	mp_option[OPT_ANALOGJOYSTICK].value = 1;
	mp_option[OPT_RISEBONUS].value = 1;
	mp_option[OPT_IMPPOGO].value = 1;
	mp_option[OPT_HUD].value = 1;

	CSettings Settings;
	Settings.saveDrvCfg();

	// And close this menu...
	m_restartVideo = true;
}

