/*
 * CMainMenu.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "CMainMenu.h"
#include <base/GsApp.h>
#include <widgets/GsMenuController.h>


#include "CSettingsMenu.h"
#include "CSelectionMenu.h"
#include "CLoadMenu.h"
#include "CSaveMenu.h"
#include "CHelpMenu.h"
#include "../CHighScores.h"
#include "common/CBehaviorEngine.h"

#include "core/mode/CGameMode.h"

#include "Button.h"


namespace vorticon
{


void MainMenu::createVorticonMenu( const bool openedGamePlay )
{

#if defined (SINGLEPLAYER)
	g_pBehaviorEngine->mPlayers = 1;
    GsButton *button = new GsButton( "New Game",
										new OpenMenuEvent(new CDifficultySelection) );
#else
    GsButton *button = new GsButton
    ( "New Game",
              new OpenMenuEvent
              (
                 new vorticon::CPlayersSelection
                 <NewGamePlayersEvent>
                 (true)
              )
    );
#endif

	mpMenuDialog->addControl( button );


	// TODO: Some items are still disabled, because those are not yet implemented in Galaxy

    GsButton *loadButton = new GsButton( "Load",
										new OpenMenuEvent( new CLoadMenu() ) );
	mpMenuDialog->addControl( loadButton );
	loadButton->mEnabled = true;

    GsButton *saveButton = new GsButton( "Save",
									new OpenMenuEvent( new CSaveMenu() ) );
	mpMenuDialog->addControl( saveButton );
	saveButton->mEnabled = openedGamePlay;

    mpMenuDialog->addControl(new GsButton( "Configure",
												new OpenMenuEvent( new CSettingsMenu() ) ) );

    GsButton *highscoreButton = new GsButton( "High Scores",
													new StartInfoSceneEvent( new CHighScores ) );
	mpMenuDialog->addControl( highscoreButton );
	highscoreButton->mEnabled = true;

    GsButton *infoButton = new GsButton( "Info",
											new OpenMenuEvent( new CHelpMenu() ) );
	mpMenuDialog->addControl( infoButton );
	infoButton->mEnabled = true;

    mpMenuDialog->addControl(new GsButton( "End Game", new EventEndGamePlay() ) );

    mpMenuDialog->addControl(new GsButton( "Quit", new GMQuit() ) );

	setMenuLabel("MAINMENULABEL");

}



MainMenu::MainMenu( const bool openedGamePlay ) :
VorticonMenu( GsRect<float>(0.25f, 0.23f, 0.5f, 0.5f) )
{
    createVorticonMenu(openedGamePlay);
}

}

