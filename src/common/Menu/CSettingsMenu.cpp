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
#include "sdl/CTimer.h"
#include "sdl/CVideoDriver.h"
#include "gui/CGUIButton.h"

#include "core/CGameControl.h"


#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CSettingsMenu::CSettingsMenu( const Uint8 dlgtheme ) :
CBaseMenu( dlgtheme, CRect<float>(0.25f, 0.24f, 0.5f, 0.32f) )
{
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	const CGUIButton::Style buttonStyle = CGUIButton::VORTICON;

	mpMenuDialog->addControl(new CGUIButton( "Video",
									new OpenMenuEvent( new CVideoSettings(dlgtheme) ),
									buttonStyle ) );

	mpMenuDialog->addControl(new CGUIButton( "Audio",
									new OpenMenuEvent( new CAudioSettings(dlgtheme) ),
									buttonStyle ) );

	mpMenuDialog->addControl(new CGUIButton( "Options",
									new OpenMenuEvent( new COptions(dlgtheme) ),
									buttonStyle ) );

	mpMenuDialog->addControl(new CGUIButton( "Controls",
									new OpenMenuEvent( new CPlayerControl(dlgtheme) ),
									buttonStyle ) );

	mpMenuDialog->addControl(new CGUIButton( "Profile",
									new OpenMenuEvent( new CProfilesMenu(dlgtheme) ),
									buttonStyle ) );

}


