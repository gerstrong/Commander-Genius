/*
 * CSettingsMenu.cpp
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#include <base/GsTimer.h>
#include <widgets/GsButton.h>
#include <widgets/GsMenuController.h>

#include "GalaxyMenu.h"

#include "SettingsMenu.h"

#include "CVideoSettings.h"
#include "CameraSettings.h"
#include "AudioSettings.h"
#include "Options.h"
#include "ControlSettings.h"
#include "CProfilesMenu.h"
#include "SelectionMenu.h"
#include "cheatsmenu.h"

namespace galaxy
{

SettingsMenu::SettingsMenu() :
GalaxyMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.40f) )
{

    mpMenuDialog->addControl(new GalaxyButton( "Video",
                                    new OpenMenuEvent( new CVideoSettings() ) ) );

    mpMenuDialog->addControl(new GalaxyButton( "Audio",
                                    new OpenMenuEvent( new CAudioSettings() ) ) );

    mpMenuDialog->addControl(new GalaxyButton( "Options",
                                    new OpenMenuEvent( new COptions() ) ) );


#if !defined(EMBEDDED)
#if defined (SINGLEPLAYER)
    mpMenuDialog->addControl( new GalaxyButton( "Controls",
                                    new OpenMenuEvent(	new CControlsettings(1) ) ) );
#else
    mpMenuDialog->addControl(new GalaxyButton( "Controls",
                                    new OpenMenuEvent( new CPlayersSelection<OpenControlMenuEvent>() ) ) );
#endif
#endif

#if !defined(EMBEDDED)
    mpMenuDialog->addControl( new GalaxyButton( "Camera",
                                    new OpenMenuEvent( new CCameraSettings() ) ) );
#endif

    mpMenuDialog->addControl(new GalaxyButton( "Cheats",
                                    new OpenMenuEvent( new CheatsMenu() ) ) );


    setMenuLabel("CONFIGMENULABEL");
}

}
