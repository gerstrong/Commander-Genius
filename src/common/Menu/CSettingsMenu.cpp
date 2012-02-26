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

CSettingsMenu::CSettingsMenu( Uint8 dlgtheme )
: CBaseMenu(dlgtheme),
m_ExeFile(g_pBehaviorEngine->m_ExeFile),
mp_option(g_pBehaviorEngine->m_option)
{
	mpMenuDialog = new CGUIDialog(CRect<float>(0.25f, 0.24f, 0.5f, 0.5f));
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	mpMenuDialog->addControl(new CGUIButton( "Graphics",
									new GMQuit(),
									CGUIButton::VORTICON ),
								CRect<float>(0.05f, 0.08f, 0.9f, 0.07f) );

	mpMenuDialog->addControl(new CGUIButton( "Audio",
									new GMQuit(),
									CGUIButton::VORTICON ),
								CRect<float>(0.05f, 0.16f, 0.9f, 0.07f) );

	mpMenuDialog->addControl(new CGUIButton( "Options",
									new GMQuit(),
									CGUIButton::VORTICON ),
								CRect<float>(0.05f, 0.24f, 0.9f, 0.07f) );

	mpMenuDialog->addControl(new CGUIButton( "Controls",
									new GMQuit(),
									CGUIButton::VORTICON ),
								CRect<float>(0.05f, 0.32f, 0.9f, 0.07f) );

	mpMenuDialog->addControl(new CGUIButton( "Profile",
									new GMQuit(),
									CGUIButton::VORTICON ),
								CRect<float>(0.05f, 0.40f, 0.9f, 0.07f) );

}


