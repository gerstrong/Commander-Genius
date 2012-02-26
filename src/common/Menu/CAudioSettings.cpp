/*
 * CAudioSettings.cpp
 *
 *  Created on: 28.11.2009
 *      Author: gerstrong
 */

#include "CAudioSettings.h"
#include "StringUtils.h"

#include "gui/CGUIButton.h"

#include "core/CGameControl.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CAudioSettings::CAudioSettings(Uint8 dlg_theme) :
CBaseMenu(dlg_theme)
{
	mpMenuDialog = new CGUIDialog(CRect<float>(0.25f, 0.24f, 0.5f, 0.5f));
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	mpMenuDialog->addControl(new CGUIButton( "Quit",
										     new GMQuit(),
										     CGUIButton::VORTICON ),
								CRect<float>(0.05f, 0.08f, 0.9f, 0.07f) );

}
