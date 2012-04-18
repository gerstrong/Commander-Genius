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
#include "engine/infoscenes/CHighScores.h"
#include "common/CBehaviorEngine.h"
#include "gui/CGUIButton.h"
#include "core/mode/CGameMode.h"


CMainMenu::CMainMenu( const bool openedGamePlay ) :
CBaseMenu( CRect<float>(0.25f, 0.24f, 0.5f, 0.5f) )
{
	CGUIButton *button = new CGUIButton( "New Game", new StartGameplayEvent(), CGUIButton::VORTICON );
	mpMenuDialog->addControl( button );


	CGUIButton *loadButton = new CGUIButton( "Load",
										new OpenMenuEvent( new CLoadMenu() ),
										CGUIButton::VORTICON );
	mpMenuDialog->addControl( loadButton );

	CGUIButton *saveButton = new CGUIButton( "Save",
									new OpenMenuEvent( new CSaveMenu() ),
									CGUIButton::VORTICON );
	mpMenuDialog->addControl( saveButton );
	saveButton->mEnabled = openedGamePlay;

	mpMenuDialog->addControl(new CGUIButton( "Settings",
												new OpenMenuEvent( new CSettingsMenu() ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "High Scores",
												new StartInfoSceneEvent( new CHighScores ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Info",
												new OpenMenuEvent( new CHelpMenu() ),
												CGUIButton::VORTICON ) );

	mpMenuDialog->addControl(new CGUIButton( "Quit", new GMQuit(), CGUIButton::VORTICON ) );

}
