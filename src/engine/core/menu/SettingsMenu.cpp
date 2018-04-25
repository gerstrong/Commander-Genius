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
//#include "CameraSettings.h"
#include "AudioSettings.h"
#include "Options.h"
#include "ControlSettings.h"
#include "CProfilesMenu.h"
#include "SelectionMenu.h"
#include "cheatsmenu.h"


SettingsMenu::SettingsMenu(const GsControl::Style &style) :
GameMenu( GsRect<float>(0.25f, 0.24f, 0.5f, 0.40f), 
          style )
{

    mpMenuDialog->addControl(new GameButton( "Display",
                                             new OpenMenuEvent( new DisplaySettings(Style()) ),
                                             style ) );

    mpMenuDialog->addControl(new GameButton( "Audio",
                                             new OpenMenuEvent( new CAudioSettings(Style()) ),
                                             style ) );


    mpMenuDialog->addControl(new GameButton( "Video",
                                             new OpenMenuEvent( new CVideoSettings(Style()) ),
                                             style ) );


    mpMenuDialog->addControl(new GameButton( "Options",
                                             new OpenMenuEvent( new COptions(Style()) ),
                                             style ) );

    if(style != GsControl::Style::NONE)
    {


#if defined (SINGLEPLAYER)
    mpMenuDialog->addControl( new GameButton( "Controls",
                                              new OpenMenuEvent(	new CControlsettings(1,
                                                                             style) ),
                                              style ) );
#else
    mpMenuDialog->addControl(new GameButton( "Controls",
                                             new OpenMenuEvent( new CPlayersSelection<OpenControlMenuEvent>(false, 
                                                                                                            style) ),
                                             style ) );
#endif
/*
#if !defined(EMBEDDED)
    mpMenuDialog->addControl( new GameButton( "Camera",
                                              new OpenMenuEvent( new CCameraSettings(style) ),
                                              style ) );
#endif
*/
    mpMenuDialog->addControl(new GameButton( "Cheats",
                                             new OpenMenuEvent( new CheatsMenu(style) ),
                                             style ) );

    }

    setMenuLabel("CONFIGMENULABEL");
}

