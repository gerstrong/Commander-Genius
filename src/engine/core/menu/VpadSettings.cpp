/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "VpadSettings.h"
#include <base/utils/StringUtils.h>
#include <base/CInput.h>
#include <base/video/CVideoDriver.h>

#include "engine/core/CBehaviorEngine.h"
#include "engine/core/CSettings.h"
#include "engine/core/VGamepads/vgamepadsimple.h"


VPadSettingsMenu::VPadSettingsMenu(const Style &style) :
GameMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f), style )
{    
    setMenuLabel("KEYBMENULABEL");

    mUsersConf = gVideoDriver.getVidConfig();

    mpVPadSwitch  = new Switch( "VirtPad", style );
    mpMenuDialog->add( mpVPadSwitch );

    const auto iSizeFac = mUsersConf.mVPadSize;

    mpVPadSize =
        mpMenuDialog->add(
            new NumberControl( "Size", 100, 400, 10, iSizeFac,
                              false, getStyle() ) );

    mpMenuDialog->fit();
    select(1);
}


void VPadSettingsMenu::refresh()
{
    mUsersConf = gVideoDriver.getVidConfig();

    mpVPadSwitch->enable(mUsersConf.mVPad);
    mpVPadSize->setSelection(mUsersConf.mVPadSize);
}


void VPadSettingsMenu::ponder(const float /*deltaT*/)
{
    GameMenu::ponder(0);

    auto &activeCfg = gVideoDriver.getVidConfig();

    activeCfg.mVPad = mpVPadSwitch->isEnabled();
    activeCfg.mVPadSize = mpVPadSize->getSelection();
}


void VPadSettingsMenu::release()
{
    mUsersConf = gVideoDriver.getVidConfig();

    gVideoDriver.setVidConfig(mUsersConf);

    gSettings.saveDrvCfg();
}

