/*
 * CSettingsMenu.cpp
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#include <lib/base/GsTimer.h>
#include <widgets/GsButton.h>
#include <widgets/GsMenuController.h>


#include "CSettingsMenu.h"
#include "CVideoSettings.h"
#include "CCameraSettings.h"
#include "CAudioSettings.h"
#include "COptions.h"
#include "CControlsettings.h"
#include "CProfilesMenu.h"
#include "CSelectionMenu.h"

//#include "sdl/CVideoDriver.h"

#include "widgets/Button.h"

namespace vorticon
{

CSettingsMenu::CSettingsMenu() :
VorticonMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.32f) )
{
 	mpMenuDialog->addControl(new GsButton( "Video",
									new OpenMenuEvent( new CVideoSettings() ) ) );

    mpMenuDialog->addControl(new GsButton( "Audio",
                                    new OpenMenuEvent( new CAudioSettings() ) ) );

	mpMenuDialog->addControl(new GsButton( "Options",
									new OpenMenuEvent( new COptions() ) ) );


	/*

			const int players = ctrlMenu->mSelection;
			EventContainer.pop_Event();
			EventContainer.add( new OpenMenuEvent(
									new CControlsettings(players) ) );
	 *
	 * */

#if defined (SINGLEPLAYER)
	mpMenuDialog->addControl( new GsButton( "Controls",
                                    new OpenMenuEvent(	new vorticon::CControlsettings(1) ) ) );
#else
    mpMenuDialog->addControl(new GsButton( "Controls",
                                    new OpenMenuEvent( new vorticon::CPlayersSelection<OpenControlMenuEvent>() ) ) );
#endif


#if !defined(EMBEDDED)
	mpMenuDialog->addControl( new GsButton( "Camera",
									new OpenMenuEvent( new CCameraSettings() ) ) );
#endif

	mpMenuDialog->addControl(new GsButton( "Profile",
									new OpenMenuEvent( new CProfilesMenu() ) ) );

	setMenuLabel("CONFIGMENULABEL");
}

}
