/*
 * CMainMenu.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "CMainMenu.h"
#include "common/Menu/CMenuController.h"
#include "common/Menu/CSettingsMenu.h"
#include "common/Menu/CSelectionMenu.h"
#include "common/Menu/CLoadMenu.h"
#include "common/Menu/CSaveMenu.h"
#include "common/Menu/CHelpMenu.h"
#include "engine/infoscenes/CHighScores.h"
#include "common/CBehaviorEngine.h"
#include "gui/CGUIButton.h"
#include "core/mode/CGameMode.h"
#include "core/CGameLauncherMenu.h"


void CMainMenu::createVorticonMenu( const bool openedGamePlay )
{

#if defined (SINGLEPLAYER)
	g_pBehaviorEngine->mPlayers = 1;
	CGUIButton *button = new CGUIButton( "New Game",
										new OpenMenuEvent(new CDifficultySelection) );
#else
	CGUIButton *button = new CGUIButton( "New Game",
									new OpenMenuEvent( new CPlayersSelection<NewGamePlayersEvent>(true) ) );

#endif

	mpMenuDialog->addControl( button );


	// TODO: Some items are still disabled, because those are not yet implemented in Galaxy

	CGUIButton *loadButton = new CGUIButton( "Load",
										new OpenMenuEvent( new CLoadMenu() ) );
	mpMenuDialog->addControl( loadButton );
	loadButton->mEnabled = true;

	CGUIButton *saveButton = new CGUIButton( "Save",
									new OpenMenuEvent( new CSaveMenu() ) );
	mpMenuDialog->addControl( saveButton );
	saveButton->mEnabled = openedGamePlay;

	mpMenuDialog->addControl(new CGUIButton( "Configure",
												new OpenMenuEvent( new CSettingsMenu() ) ) );

	CGUIButton *highscoreButton = new CGUIButton( "High Scores",
													new StartInfoSceneEvent( new CHighScores ) );
	mpMenuDialog->addControl( highscoreButton );
	highscoreButton->mEnabled = true;

	CGUIButton *infoButton = new CGUIButton( "Info",
											new OpenMenuEvent( new CHelpMenu() ) );
	mpMenuDialog->addControl( infoButton );
	infoButton->mEnabled = true;

	mpMenuDialog->addControl(new CGUIButton( "End Game", new EventEndGamePlay() ) );

	mpMenuDialog->addControl(new CGUIButton( "Quit", new GMQuit() ) );

	setMenuLabel("MAINMENULABEL");

}


void CMainMenu::createGalaxyMenu( const bool openedGamePlay )
{
	g_pBehaviorEngine->mPlayers = 1;

	CGUIButton *button = new CGUIButton( "New Game",
										new OpenMenuEvent(new CDifficultySelection) );
	mpMenuDialog->addControl( button );


	// TODO: Some items are still disabled, because those are not yet implemented in Galaxy

	CGUIButton *loadButton = new CGUIButton( "Load",
										new OpenMenuEvent( new CLoadMenu() ) );
	mpMenuDialog->addControl( loadButton );
	loadButton->mEnabled = true;

	CGUIButton *saveButton = new CGUIButton( "Save",
									new OpenMenuEvent( new CSaveMenu() ) );
	mpMenuDialog->addControl( saveButton );
	saveButton->mEnabled = openedGamePlay;

	mpMenuDialog->addControl(new CGUIButton( "Configure",
												new OpenMenuEvent( new CSettingsMenu() ) ) );

	CGUIButton *highscoreButton = new CGUIButton( "High Scores",
													new StartInfoSceneEvent( new CHighScores ) );
	mpMenuDialog->addControl( highscoreButton );
	highscoreButton->mEnabled = false;

	CGUIButton *infoButton = new CGUIButton( "Info",
											new OpenMenuEvent( new CHelpMenu() ) );
	mpMenuDialog->addControl( infoButton );
	infoButton->mEnabled = false;

	mpMenuDialog->addControl(new CGUIButton( "End Game", new EventEndGamePlay() ) );

	mpMenuDialog->addControl(new CGUIButton( "Quit", new GMQuit() ) );

	setMenuLabel("MAINMENULABEL");

}



CMainMenu::CMainMenu( const bool openedGamePlay ) :
CBaseMenu( CRect<float>(0.25f, 0.23f, 0.5f, 0.5f) )
{
	if(g_pBehaviorEngine->getEngine() == ENGINE_GALAXY)
	{
		createGalaxyMenu(openedGamePlay);
	}
	else if(g_pBehaviorEngine->getEngine() == ENGINE_VORTICON)
	{
		createVorticonMenu(openedGamePlay);
	}
}
