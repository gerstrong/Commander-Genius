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
#include "engine/core/CBehaviorEngine.h"

#include "engine/core/mode/CGameMode.h"

#include "widgets/Button.h"


namespace vorticon
{


void MainMenu::createVorticonMenu( const bool openedGamePlay )
{

#if defined (SINGLEPLAYER)
	g_pBehaviorEngine->mPlayers = 1;
    Button *button = new Button( "New Game",
										new OpenMenuEvent(new CDifficultySelection) );
#else
    Button *button = new Button
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

    GsButton *loadButton = new Button( "Load",
										new OpenMenuEvent( new CLoadMenu() ) );
	mpMenuDialog->addControl( loadButton );
    loadButton->enable(true);

    Button *saveButton = new Button( "Save",
									new OpenMenuEvent( new CSaveMenu() ) );
	mpMenuDialog->addControl( saveButton );
    saveButton->enable(openedGamePlay);

    mpMenuDialog->addControl(new Button( "Configure",
												new OpenMenuEvent( new CSettingsMenu() ) ) );

    GsButton *highscoreButton = new Button( "High Scores",
													new StartInfoSceneEvent( new CHighScores ) );
	mpMenuDialog->addControl( highscoreButton );
    highscoreButton->enable(true);

    GsButton *infoButton = new Button( "Info",
											new OpenMenuEvent( new CHelpMenu() ) );
	mpMenuDialog->addControl( infoButton );
    infoButton->enable(true);

    mpMenuDialog->addControl(new Button( "End Game", new EventEndGamePlay() ) );

    mpMenuDialog->addControl(new Button( "Quit", new GMQuit() ) );

}



MainMenu::MainMenu( const bool openedGamePlay ) :
VorticonMenu( GsRect<float>(0.25f, 0.23f, 0.5f, 0.5f) )
{
    createVorticonMenu(openedGamePlay);
}

}

