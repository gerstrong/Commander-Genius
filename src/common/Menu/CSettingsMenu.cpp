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
#include "CProfilesMenu.h"
#include "CSelectionMenu.h"
#include "sdl/CTimer.h"
#include "sdl/CVideoDriver.h"
#include "gui/CGUIButton.h"

#include "core/CGameControl.h"


CSettingsMenu::CSettingsMenu() :
CBaseMenu( CRect<float>(0.25f, 0.24f, 0.5f, 0.32f) )
{
 	mpMenuDialog->addControl(new CGUIButton( "Video",
									new OpenMenuEvent( new CVideoSettings() ) ) );

	mpMenuDialog->addControl(new CGUIButton( "Audio",
									new OpenMenuEvent( new CAudioSettings() ) ) );

	mpMenuDialog->addControl(new CGUIButton( "Options",
									new OpenMenuEvent( new COptions() ) ) );


	/*

			const int players = ctrlMenu->mSelection;
			EventContainer.pop_Event();
			EventContainer.add( new OpenMenuEvent(
									new CControlsettings(players) ) );
	 *
	 * */

#if defined (SINGLEPLAYER)
	mpMenuDialog->addControl( new CGUIButton( "Controls",
									new OpenMenuEvent(	new CControlsettings(1) ) ) );
#else
	mpMenuDialog->addControl(new CGUIButton( "Controls",
									new OpenMenuEvent( new CPlayersSelection<OpenControlMenuEvent>() ) ) );
#endif

	mpMenuDialog->addControl(new CGUIButton( "Profile",
									new OpenMenuEvent( new CProfilesMenu() ) ) );

	setMenuLabel("CONFIGMENULABEL");
}


