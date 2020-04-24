
/*
 * CAudioSettings.h
 *
 *  Created on: 03.03.2009
 *      Author: gerstrong
 */

#ifndef CVGAMEPADSETTINGS_H
#define CVGAMEPADSETTINGS_H

#include "engine/core/menu/GameMenu.h"

#include "widgets/NumberControl.h"
#include "widgets/Switch.h"

#include <base/video/CVidConfig.h>

#include <SDL.h>
#include <string>



class OverlaySettings : public GameMenu
{
public:
    OverlaySettings(const Style &style);

    void init();

    void ponder(const float) override;

    void release() override;

    void refresh() override;

private:

    CVidConfig mUsersConf;

    std::shared_ptr<Switch>         mpShowCursorSwitch;


#ifdef VIRTUALPAD
    Switch			*mpVPadSwitch  = nullptr;
#endif


    std::shared_ptr<NumberControl>	mpVPadSize;
};


#endif /* CVGAMEPADSETTINGS_H */
