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
CBaseMenu( dlgtheme, CRect<float>(0.25f, 0.24f, 0.5f, 0.35f) )
{
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	const CGUIButton::Style buttonStyle = CGUIButton::VORTICON;

	mpMenuDialog->addControl(new CGUIButton( "Video",
									new OpenMenuEvent( new CVideoSettings(dlgtheme) ),
									buttonStyle ),
								CRect<float>(0.05f, 0.13f, 0.9f, 0.12f) );

	mpMenuDialog->addControl(new CGUIButton( "Audio",
									new OpenMenuEvent( new CAudioSettings(dlgtheme) ),
									buttonStyle ),
								CRect<float>(0.05f, 0.26f, 0.9f, 0.12f) );

	mpMenuDialog->addControl(new CGUIButton( "Options",
									new OpenMenuEvent( new COptions(dlgtheme) ),
									buttonStyle ),
								CRect<float>(0.05f, 0.39f, 0.9f, 0.12f) );

	mpMenuDialog->addControl(new CGUIButton( "Controls",
									new OpenMenuEvent( new CControlsettings(dlgtheme) ),
									buttonStyle ),
								CRect<float>(0.05f, 0.52f, 0.9f, 0.12f) );

	mpMenuDialog->addControl(new CGUIButton( "Profile",
									new OpenMenuEvent( new CProfilesMenu(dlgtheme) ),
									buttonStyle ),
								CRect<float>(0.05f, 0.65f, 0.9f, 0.12f) );

}


