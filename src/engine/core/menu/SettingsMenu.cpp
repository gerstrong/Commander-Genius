/*
 * CSettingsMenu.cpp
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#include <base/GsTimer.h>
#include <widgets/GsButton.h>
#include <widgets/GsMenuController.h>

#include "GameMenu.h"

#include "SettingsMenu.h"

#include "GameSpecSettings.h"
#include "DisplaySettings.h"
#include "OverlaySettings.h"
#include "AudioSettings.h"
#include "Options.h"
#include "ControlSettings.h"
//#include "CProfilesMenu.h"
#include "SelectionMenu.h"
#include "cheatsmenu.h"


SettingsMenu::SettingsMenu(const Style &style, const std::string_view &engineName) :
GameMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.40f),
          style )
{

    mpMenuDialog->add(
                new GameButton( "Display",
                                new OpenMenuEvent(
                                    new DisplaySettings(style) ),
                                style ) );


    const std::string engineButtonName =
        (engineName == "Launcher") ? "Launcher" : "Game";
    mpMenuDialog->add(
        new GameButton( engineButtonName,
                       new OpenMenuEvent(
                           new GameSpecSettings(style, engineName) ),
                       style ) );

    mpMenuDialog->add(
                new GameButton( "Audio",
                                new OpenMenuEvent(
                                    new CAudioSettings(style) ),
                                style ) );


    mpMenuDialog->add(
                new GameButton( "Options",
                                new OpenMenuEvent(
                                    new COptions(style) ),
                                style ) );

    if(style != Style::NONE)
    {


#if defined (SINGLEPLAYER)
        mpMenuDialog->add(
                    new GameButton( "Controls",
                                   new OpenMenuEvent(
                                        new CControlsettings(1,style) ),
                                                  style ) );
#else
        mpMenuDialog->add(
               new GameButton( "Controls",
                   new OpenMenuEvent(
                       new CPlayersSelection<OpenControlMenuEvent>(false,style) ),
                                                 style ) );
#endif

    }

    mpMenuDialog->add(new GameButton( "Overlay",
                                             new OpenMenuEvent( new OverlaySettings(style) ),
                                             style ) );

    if(style != Style::NONE)
    {

        mpMenuDialog->add(new GameButton( "Cheats",
                                                 new OpenMenuEvent( new CheatsMenu(style) ),
                                                 style ) );

    }


    setMenuLabel("CONFIGMENULABEL");

    mpMenuDialog->fit();
    select(1);
}

