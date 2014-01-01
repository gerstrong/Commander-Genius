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
#include "engine/CEvent.h"
#include <list>
#include <string>


/**
 * \brief This sets the default settings for a classic gameplay
 */
class SetDefaultClassic : public InvokeFunctorEvent
{
public:
    void operator()() const
	{
		stOption *p_option = g_pBehaviorEngine->m_option;
		g_pSettings->loadDrvCfg();
		g_pVideoDriver->setSpecialFXMode(false);
		st_camera_bounds &CameraBounds = g_pVideoDriver->getCameraBounds();
		CameraBounds.left = 140;
		CameraBounds.up = 50;
		CameraBounds.right = 180;
		CameraBounds.down = 130;
		CameraBounds.speed = 5;
		g_pTimer->setFPS(30);
		p_option[OPT_ALLOWPKING].value = 0;
		p_option[OPT_KEYSTACK].value = 0;
		p_option[OPT_LVLREPLAYABILITY].value = 0;
        p_option[OPT_RISEBONUS].value = 0;
        p_option[OPT_MODERN].value = 1;
        p_option[OPT_HUD].value = 0;
		g_pSettings->saveDrvCfg();
	}
};


/**
 * \brief This sets the default settings for an enhanced gameplay
 */
class SetDefaultEnhanced : public InvokeFunctorEvent
{
public:
    void operator()() const
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
		g_pTimer->setFPS(60);
		p_option[OPT_KEYSTACK].value = 1;
		p_option[OPT_RISEBONUS].value = 1;
        p_option[OPT_MODERN].value = 1;
		p_option[OPT_HUD].value = 1;
		g_pSettings->saveDrvCfg();
	}
};

CProfilesMenu::CProfilesMenu() :
CBaseMenu( GsRect<float>(0.25f, 0.4f, 0.5f, 0.2f) )
{
	mpMenuDialog->addControl(new CGUIButton( "Classic mode",
										     new SetDefaultClassic() ) );

	mpMenuDialog->addControl(new CGUIButton( "Enhanced mode",
										     new SetDefaultEnhanced() ) );

	setMenuLabel("OPTIONSMENULABEL");
}

