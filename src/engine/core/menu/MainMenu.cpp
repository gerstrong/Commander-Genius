/*
 * CMainMenu.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong and pizza2004
 */

#include "MainMenu.h"
#include <base/CInput.h>
#include <base/GsApp.h>
#include <widgets/GsMenuController.h>


#include "SettingsMenu.h"
#include "SelectionMenu.h"
#include "LoadMenu.h"
#include "SaveMenu.h"
#include "achievementmenu.h"
#include "QuitMenu.h"
#include "engine/core/CBehaviorEngine.h"

#include "widgets/Button.h"
#include "engine/core/mode/Scene.h"
#include "SelectionMenu.h"
#include "CHelpMenu.h"

void MainMenu::createGameMenu( const bool openedGamePlay, 
                               const Style style,
                               const bool singlePlayer,
                               const std::string_view &engineName )
{
    GsButton *button = nullptr;

    if(singlePlayer)
    {
        gBehaviorEngine.setNumPlayers(1);

        button = new GameButton( "New Game",
                                 new NewGamePlayersEvent(1), style );
    }
    else
    {
        button = new GameButton( "New Game",
                 new OpenMenuEvent(
                     new CPlayersSelection<NewGamePlayersEvent>(true, style) ), style );
    }

	mpMenuDialog->add( button );

	// TODO: Some items are still disabled, because those are not yet implemented in Galaxy

    auto loadButton =
            mpMenuDialog->add( new GameButton( "Load",
                                               new OpenMenuEvent( new CLoadMenu(style) ),
                                               style ) );
    loadButton->enable(true);

    auto saveButton =
            mpMenuDialog->add( new GameButton( "Save",
                                               new OpenMenuEvent( new CSaveMenu(style) ),
                                               style ) );
    saveButton->enable(openedGamePlay);

    mpMenuDialog->add(new GameButton( "Configure",
                                             new OpenMenuEvent( new SettingsMenu(style, engineName) ),
                                             style) );

    if(style == Style::VORTICON || style == Style::NONE)
    {
        GsButton *highscoreButton = new GameButton( "High Scores",
                                                    new EventHighScores(),
                                                    style);

        mpMenuDialog->add( highscoreButton );
        highscoreButton->enable(true);
    }

/*
    {


        GsButton *infoButton = new GameButton( "Info",
                                               new OpenMenuEvent( new CHelpMenu() ) );
        mpMenuDialog->add( infoButton );
        infoButton->enable(false);
    }*/

    if(style == Style::GALAXY)
    {

        mpMenuDialog->add(
                    new GameButton( "Achievements",
                                    new OpenMenuEvent(
                                        new AchievementMenu(0, style) ),
                                    style) );
    }


    if(style == Style::VORTICON || style == Style::NONE)
    {
        mpMenuDialog->add(new GameButton( "Info",
                                          new OpenMenuEvent( new CHelpMenu(style) ),
                                          style) );        
    }

    mpMenuDialog->add(new GameButton( "Quit",
                                      new OpenMenuEvent(
                                          new QuitMenu(openedGamePlay, style) ),
                                      style ) );

    setMenuLabel("MAINMENULABEL");

    mpMenuDialog->fit();
    select(1);
}

void MainMenu::ponder(const float dt)
{
    // If IC_BACK is invoked, make the menu controller close the controller
    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseMenuEvent(true) );
        return;
    }

    handleInput(dt);

    mpMenuDialog->processLogic();
}

MainMenu::MainMenu( const bool openedGamePlay, 
                    const Style style,
                    const bool singlePlayer,
                    const std::string_view &engineName ) :
GameMenu( GsRect<float>(0.25f, 0.23f, 0.5f, 0.5f),
          style, true)
{   
    createGameMenu(openedGamePlay, style, singlePlayer, engineName);
}

