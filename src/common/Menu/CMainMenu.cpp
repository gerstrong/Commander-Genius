/*
 * CMainMenu.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "CMainMenu.h"
#include "common/Menu/CMenuController.h"
#include "common/Menu/CSettingsMenu.h"
#include "common/Menu/CLoadMenu.h"
#include "common/Menu/CSaveMenu.h"
#include "common/Menu/CHelpMenu.h"
#include "common/CBehaviorEngine.h"
#include "gui/CGUIButton.h"
#include "core/mode/CGameMode.h"


CMainMenu::CMainMenu( const Uint8 dlgTheme ) :
CBaseMenu( dlgTheme, CRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{

	// Create the Menu Dialog and entries
	mpMenuDialog->setBackground(CGUIDialog::VORTICON);

	CGUIButton *button = new CGUIButton( "New Game", new StartGameplayEvent(), CGUIButton::VORTICON );
	mpMenuDialog->addControl( button );

	mpMenuDialog->addControl(new CGUIButton( "Load",
												new OpenMenuEvent( new CLoadMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Save",
												new OpenMenuEvent( new CSaveMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Settings",
												new OpenMenuEvent( new CSettingsMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "High Scores",
												new StartHighscoresEvent(),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Info",
												new OpenMenuEvent( new CHelpMenu(dlgTheme) ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Quit", new GMQuit(), CGUIButton::VORTICON ) );

}
