/*
 * CMainMenu.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "CMainMenu.h"
#include <base/GsApp.h>
#include <widgets/GsMenuController.h>


#include "common/Menu/CSettingsMenu.h"
#include "common/Menu/CSelectionMenu.h"
#include "common/Menu/CLoadMenu.h"
#include "common/Menu/CSaveMenu.h"
#include "common/Menu/CHelpMenu.h"
#include "engine/infoscenes/CHighScores.h"
#include "common/CBehaviorEngine.h"

#include "core/mode/CGameMode.h"

#include "engine/galaxy/menu/GalaxyButton.h"


void CMainMenu::createVorticonMenu( const bool openedGamePlay )
{

#if defined (SINGLEPLAYER)
	g_pBehaviorEngine->mPlayers = 1;
    GsButton *button = new GsButton( "New Game",
										new OpenMenuEvent(new CDifficultySelection) );
#else
    GsButton *button =
            new GsButton( "New Game",
                new OpenMenuEvent( new CPlayersSelection<NewGamePlayersEvent,
                                                         GalaxyButton>(true) ) );
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


void CMainMenu::createGalaxyMenu( const bool openedGamePlay )
{
#if defined (SINGLEPLAYER)
    g_pBehaviorEngine->mPlayers = 1;
    GsButton *button = new GalaxyButton( "New Game",
                                        new OpenMenuEvent(new CDifficultySelection) );
#else
    GsButton *button = new GalaxyButton( "New Game",
             new OpenMenuEvent( new CPlayersSelection<NewGamePlayersEvent, GalaxyButton>(true) ) );
#endif

	mpMenuDialog->addControl( button );

	// TODO: Some items are still disabled, because those are not yet implemented in Galaxy

    GsButton *loadButton = new GalaxyButton( "Load",
										new OpenMenuEvent( new CLoadMenu() ) );
	mpMenuDialog->addControl( loadButton );
	loadButton->mEnabled = true;

    GsButton *saveButton = new GalaxyButton( "Save",
									new OpenMenuEvent( new CSaveMenu() ) );
	mpMenuDialog->addControl( saveButton );
	saveButton->mEnabled = openedGamePlay;

    mpMenuDialog->addControl(new GalaxyButton( "Configure",
												new OpenMenuEvent( new CSettingsMenu() ) ) );

    GsButton *highscoreButton = new GalaxyButton( "High Scores",
													new StartInfoSceneEvent( new CHighScores ) );
	mpMenuDialog->addControl( highscoreButton );
	highscoreButton->mEnabled = false;

    GsButton *infoButton = new GalaxyButton( "Info",
											new OpenMenuEvent( new CHelpMenu() ) );
	mpMenuDialog->addControl( infoButton );
	infoButton->mEnabled = false;

    mpMenuDialog->addControl(new GalaxyButton( "End Game", new EventEndGamePlay() ) );

    mpMenuDialog->addControl(new GalaxyButton( "Quit", new GMQuit() ) );

	setMenuLabel("MAINMENULABEL");

}



CMainMenu::CMainMenu( const bool openedGamePlay ) :
CBaseMenu( GsRect<float>(0.25f, 0.23f, 0.5f, 0.5f) )
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
