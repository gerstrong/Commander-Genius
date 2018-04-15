/*
 * CProfilesMenu.cpp
 *
 *  Created on: 14.08.2010
 *      Author: gerstrong
 */

#include "CProfilesMenu.h"
#include "engine/keen/menu/CSelectionMenu.h"
#include "engine/core/CSettings.h"
#include <base/video/CVideoDriver.h>
#include <base/GsTimer.h>
#include "engine/core/CBehaviorEngine.h"
#include <base/GsEvent.h>
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
        auto &options = gBehaviorEngine.mOptions;
        gSettings.loadDrvCfg();
		st_camera_bounds &CameraBounds = gVideoDriver.getCameraBounds();
		CameraBounds.left = 140;
		CameraBounds.up = 50;
		CameraBounds.right = 180;
		CameraBounds.down = 130;
		CameraBounds.speed = 5;
		gTimer.setFPS(30);
        options[GameOption::ALLOWPKING].value = 0;
        options[GameOption::KEYSTACK].value = 0;
        options[GameOption::LVLREPLAYABILITY].value = 0;
        options[GameOption::RISEBONUS].value = 0;
        options[GameOption::MODERN].value = 1;
        options[GameOption::HUD].value = 0;
        options[GameOption::SPECIALFX].value = 0;
        gSettings.saveDrvCfg();
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
        auto &options = gBehaviorEngine.mOptions;
        gSettings.loadDrvCfg();
		st_camera_bounds &CameraBounds = gVideoDriver.getCameraBounds();
		CameraBounds.left = 152;
		CameraBounds.up = 92;
		CameraBounds.right = 168;
		CameraBounds.down = 108;
		CameraBounds.speed = 20;
		gTimer.setFPS(60);
        options[GameOption::KEYSTACK].value = 1;
        options[GameOption::RISEBONUS].value = 1;
        options[GameOption::MODERN].value = 1;
        options[GameOption::HUD].value = 1;
        options[GameOption::SPECIALFX].value = 1;
        gSettings.saveDrvCfg();
	}
};

CProfilesMenu::CProfilesMenu(const GsControl::Style style) :
GameMenu( GsRect<float>(0.25f, 0.4f, 0.5f, 0.2f), style )
{
    mpMenuDialog->addControl(new GameButton( "Classic mode",
                                             new SetDefaultClassic(),
                                             style) );

    mpMenuDialog->addControl(new GameButton( "Enhanced mode",
                                             new SetDefaultEnhanced(),
                                             style) );

	setMenuLabel("OPTIONSMENULABEL");
}


