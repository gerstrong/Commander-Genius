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


VPadSettingsMenu::VPadSettingsMenu(const GsControl::Style &style) :
GameMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f), style )
{    

    if( gVideoDriver.VGamePadEnabled() )
    {
        VirtualKeenControl *vkc = dynamic_cast<VirtualKeenControl*>(gInput.mpVirtPad.get());
        assert(vkc);

        const auto iSizeFac = gVideoDriver.getVidConfig().mVPadSize;

        mpSize = new NumberControl( "Size \%", 100, 400, 10, iSizeFac,
                                    false, style );

        mpMenuDialog->addControl( mpSize );
    }

    setMenuLabel("KEYBMENULABEL");

}


void VPadSettingsMenu::refresh()
{
}


void VPadSettingsMenu::ponder(const float /*deltaT*/)
{
    GameMenu::ponder(0);

    const auto iSize = mpSize->getSelection();

    gVideoDriver.getVidConfig().mVPadSize = iSize;
}


void VPadSettingsMenu::release()
{
    gSettings.saveDrvCfg();
}

