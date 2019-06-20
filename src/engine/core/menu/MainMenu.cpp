/*
 * CMainMenu.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "MainMenu.h"
#include <base/GsApp.h>
#include <widgets/GsMenuController.h>


#include "SettingsMenu.h"
#include "SelectionMenu.h"
#include "LoadMenu.h"
#include "SaveMenu.h"
#include "engine/core/CBehaviorEngine.h"

#include "widgets/Button.h"
#include "engine/core/mode/CGameMode.h"
#include "SelectionMenu.h"



void MainMenu::createGameMenu( const bool openedGamePlay, 
                               const Style style )
{

#if defined (SINGLEPLAYER)
    gBehaviorEngine.setNumPlayers(1);

    GsButton *button = new GameButton( "New Game", new NewGamePlayersEvent(1), style );
#else
    GsButton *button = new GameButton( "New Game",
             new OpenMenuEvent( new CPlayersSelection<NewGamePlayersEvent>(true, style) ), style );
#endif

	mpMenuDialog->addWidget( button );

	// TODO: Some items are still disabled, because those are not yet implemented in Galaxy

    GsButton *loadButton = new GameButton( "Load",
                                           new OpenMenuEvent( new CLoadMenu(style) ),
                                           style );
	mpMenuDialog->addWidget( loadButton );
    loadButton->enable(true);

    GsButton *saveButton = new GameButton( "Save",
									       new OpenMenuEvent( new CSaveMenu(style) ),
                                           style );
	mpMenuDialog->addWidget( saveButton );
    saveButton->enable(openedGamePlay);

    mpMenuDialog->addWidget(new GameButton( "Configure",
                                             new OpenMenuEvent( new SettingsMenu(style) ),
                                             style) );

/*
    {
        GsButton *highscoreButton = new GameButton( "High Scores",
                                                    new StartInfoSceneEvent( new CHighScores(style) ),
                                                    style);

        mpMenuDialog->addWidget( highscoreButton );
        highscoreButton->enable(false);


        GsButton *infoButton = new GameButton( "Info",
                                               new OpenMenuEvent( new CHelpMenu() ) );
        mpMenuDialog->addWidget( infoButton );
        infoButton->enable(false);
    }*/

    const std::string endGamePlayTest =
            openedGamePlay ? "End Game" : "Quit to Launcher";

#if !defined(EMBEDDED)

    mpMenuDialog->addWidget(new GameButton( endGamePlayTest, new EventEndGamePlay(), style ) );

#else

    if(openedGamePlay)
    {
        mpMenuDialog->addWidget(new GameButton( endGamePlayTest, new EventEndGamePlay(), style ) );
    }

#endif

    mpMenuDialog->addWidget(new GameButton( "Quit", new GMQuit(), style ) );

    setMenuLabel("MAINMENULABEL");

    mpMenuDialog->fit();
}



MainMenu::MainMenu( const bool openedGamePlay, 
                    const Style style ) :
GameMenu( GsRect<float>(0.25f, 0.23f, 0.5f, 0.5f), style )
{   
    createGameMenu(openedGamePlay, style);
}

