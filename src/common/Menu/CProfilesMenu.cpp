/*
 * CProfilesMenu.cpp
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 */

#include "CProfilesMenu.h"
#include "common/Menu/CSelectionMenu.h"
#include "common/CSettings.h"
#include "sdl/CVideoDriver.h"
#include "sdl/CTimer.h"
#include "common/CBehaviorEngine.h"
#include <list>
#include <string>

CProfilesMenu::CProfilesMenu( const Uint8 dlgTheme ) :
CBaseMenu( dlgTheme, CRect<float>(0.25f, 0.4f, 0.5f, 0.2f) )
{
	mpMenuDialog->setBackground( CGUIDialog::VORTICON );

	/*mpMenuDialog->addControl(new CGUIButton( "Quit",
										     new GMQuit(),
										     CGUIButton::VORTICON ),
								CRect<float>(0.08f, 0.08f, 0.3f, 0.07f) );*/


	/*std::list<std::string> profile_list;
	profile_list.push_back("Classic mode");
	profile_list.push_back("Enhanced mode");*/

	//mp_SubMenu = new CSelectionMenu<int>( m_selection, profile_list, m_dlg_theme );
}

/*void CProfilesMenu::processSpecific()
{*/
	/*if(mp_SubMenu.get())
	{
		mp_SubMenu->processCommon();
		mp_SubMenu->processSpecific();
		mp_SubMenu->postProcess();		
	}
	
	if( m_selection != NO_SELECTION)
	{
		if(m_selection == 1)
			setDefaultClassic();
		else if(m_selection == 2)
			setDefaultEnhanced();
		m_mustclose = true;
	}


	if(mp_SubMenu.get() && mp_SubMenu->mustClose())
	{
		mp_SubMenu = NULL;
		m_mustclose = true;
	}*/
//}

/**
 * \brief This sets the default settings for a classic gameplay
 */
/*void CProfilesMenu::setDefaultClassic()
{
	stOption *p_option = g_pBehaviorEngine->m_option;
	g_pSettings->loadDrvCfg();
	g_pVideoDriver->setSpecialFXMode(false);
	st_camera_bounds &CameraBounds = g_pVideoDriver->getCameraBounds();
	CameraBounds.left = 140;
	CameraBounds.up = 50; // Was 500, that's why camera was jumpy!
	CameraBounds.right = 180;
	CameraBounds.down = 130;
	CameraBounds.speed = 5;
	g_pTimer->setFrameRate(DEFAULT_LPS, 30, DEFAULT_SYNC);
	p_option[OPT_SUPERPOGO].value = 0;
	p_option[OPT_ALLOWPKING].value = 0;
	p_option[OPT_KEYSTACK].value = 0;
	p_option[OPT_ANALOGJOYSTICK].value = 0;
	p_option[OPT_LVLREPLAYABILITY].value = 0;
	p_option[OPT_RISEBONUS].value = 0;
	p_option[OPT_IMPPOGO].value = 0;
	p_option[OPT_HUD].value = 0;
	g_pSettings->saveDrvCfg();
}*/

/**
 * \brief This sets the default settings for an enhanced gameplay
 */
/*void CProfilesMenu::setDefaultEnhanced()
{
	stOption *p_option = g_pBehaviorEngine->m_option;
	g_pSettings->loadDrvCfg();
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
	g_pSettings->saveDrvCfg();
}*/
