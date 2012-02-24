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
	/*mp_Dialog = new CDialog(13, 7, INPUT_MODE_UP_DOWN,m_dlg_theme);

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Graphics");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Audio");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Options");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 4, "Controls");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 5, "Profile");

	m_selection = selection;*/

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

void CSettingsMenu::processSpecific()
{
	/*if(!mp_SubMenu)
	{
		if( m_selection != NO_SELECTION)
		{
			switch(m_selection)
			{
			case 0: mp_SubMenu = new CVideoSettings(m_dlg_theme, m_restartVideo); break;
			case 1: mp_SubMenu = new CAudioSettings(m_dlg_theme, m_ExeFile); break;
			case 2: mp_SubMenu = new COptions(m_dlg_theme, mp_option); break;
			case 3: mp_SubMenu = new CControlsettings(m_dlg_theme); break;
			case 4: mp_SubMenu = new CProfilesMenu(m_dlg_theme); break;
			}

			m_selection = NO_SELECTION;
			m_suspended = true;
		}
	}
	else
	{
		mp_SubMenu->processCommon();
		mp_SubMenu->processSpecific();
		mp_SubMenu->postProcess();

		if(mp_SubMenu->mustClose())
		{
			SAFE_DELETE(mp_SubMenu);
			m_suspended = false;
		}
	}*/
}

