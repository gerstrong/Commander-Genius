/*
 * CProfilesMenu.cpp
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 */

#include "CProfilesMenu.h"
#include "common/Menu/CSelectionMenu.h"
#include "sdl/CSettings.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"
#include <list>
#include <string>

CProfilesMenu::CProfilesMenu(Uint8 dlg_theme) :
CBaseMenu(dlg_theme)
{
	std::list<std::string> profile_list;
	profile_list.push_back("Classic mode (like DOS)");
	profile_list.push_back("Enhanced mode (more stuff)");

	mp_SubMenu = new CSelectionMenu<int>( m_selection, profile_list, m_dlg_theme );
}

void CProfilesMenu::processSpecific()
{
	mp_SubMenu->processCommon();
	mp_SubMenu->processSpecific();
	mp_SubMenu->postProcess();

	if( m_selection != NO_SELECTION)
	{
		if(m_selection == 1)
			setDefaultClassic();
		else if(m_selection == 2)
			setDefaultEnhanced();
		m_mustclose = true;
	}


	if(mp_SubMenu->mustClose())
	{
		delete mp_SubMenu;
		mp_SubMenu = NULL;
		m_suspended = false;
	}
}

/**
 * \brief This sets the default settings for a classic gameplay
 */
void CProfilesMenu::setDefaultClassic()
{
	stOption p_option[NUM_OPTIONS];
	CSettings Settings(p_option);
	Settings.loadDrvCfg();
	g_pVideoDriver->setSpecialFXMode(false);
	st_camera_bounds &CameraBounds = g_pVideoDriver->getCameraBounds();
	CameraBounds.left = 160;
	CameraBounds.up = 100;
	CameraBounds.right = 161;
	CameraBounds.down = 101;
	CameraBounds.speed = 10;
	g_pTimer->setFrameRate(DEFAULT_LPS, 30, DEFAULT_SYNC);
	p_option[OPT_SUPERPOGO].value = 0;
	p_option[OPT_ALLOWPKING].value = 0;
	p_option[OPT_KEYSTACK].value = 0;
	p_option[OPT_ANALOGJOYSTICK].value = 0;
	p_option[OPT_LVLREPLAYABILITY].value = 0;
	p_option[OPT_RISEBONUS].value = 0;
	p_option[OPT_IMPPOGO].value = 0;
	p_option[OPT_HUD].value = 0;
	Settings.saveDrvCfg();
}

/**
 * \brief This sets the default settings for an enhanced gameplay
 */
void CProfilesMenu::setDefaultEnhanced()
{
	stOption p_option[NUM_OPTIONS];
	CSettings Settings(p_option);
	Settings.loadDrvCfg();
	g_pVideoDriver->setSpecialFXMode(true);
	st_camera_bounds &CameraBounds = g_pVideoDriver->getCameraBounds();
	CameraBounds.left = 152;
	CameraBounds.up = 92;
	CameraBounds.right = 168;
	CameraBounds.down = 108;
	CameraBounds.speed = 20;
	g_pTimer->setFrameRate(DEFAULT_LPS, 60, DEFAULT_SYNC);
	p_option[OPT_KEYSTACK].value = 1;
	p_option[OPT_ANALOGJOYSTICK].value = 1;
	p_option[OPT_RISEBONUS].value = 1;
	p_option[OPT_IMPPOGO].value = 1;
	p_option[OPT_HUD].value = 1;
	Settings.saveDrvCfg();
}

CProfilesMenu::~CProfilesMenu()
{
	delete mp_SubMenu;
}
