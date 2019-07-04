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

#include "CVideoSettings.h"
#include "DisplaySettings.h"
#include "VpadSettings.h"
#include "AudioSettings.h"
#include "Options.h"
#include "ControlSettings.h"
#include "CProfilesMenu.h"
#include "SelectionMenu.h"
#include "cheatsmenu.h"


SettingsMenu::SettingsMenu(const Style &style) :
GameMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.40f), 
          style )
{

#if !defined(EMBEDDED)

    mpMenuDialog->add(
                new GameButton( "Display",
                                new OpenMenuEvent(
                                    new DisplaySettings(style) ),
                                style ) );


    mpMenuDialog->add(
                new GameButton( "Video",
                                new OpenMenuEvent(
                                    new CVideoSettings(style) ),
                                style ) );

#endif

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

#ifdef VIRTUALPAD
    mpMenuDialog->add(new GameButton( "Virtual Gamepad",
                                             new OpenMenuEvent( new VPadSettingsMenu(style) ),
                                             style ) );
#endif


    if(style != Style::NONE)
    {

        mpMenuDialog->add(new GameButton( "Cheats",
                                                 new OpenMenuEvent( new CheatsMenu(style) ),
                                                 style ) );

    }


    setMenuLabel("CONFIGMENULABEL");

    mpMenuDialog->fit();
}

