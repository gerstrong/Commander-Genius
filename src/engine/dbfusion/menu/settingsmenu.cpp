#include "settingsmenu.h"
#include "videosettings.h"

namespace dbfusion
{

SettingsMenu::SettingsMenu() :
    FusionMenu(GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f))
{      
    // Settingssubmenu buttons
    mpMenuDialog->addControl(new GsButton( "Video",
                                    new OpenMenuEvent( new VideoSettings() ) ),
                                        GsRect<float>(0.2f, 0.2f, 0.6f, 0.05f) );

/*    mpMenuDialog->addControl(new GalaxyButton( "Audio",
                                    new OpenMenuEvent( new CAudioSettings() ) ) );

    mpMenuDialog->addControl(new GalaxyButton( "Options",
                                    new OpenMenuEvent( new COptions() ) ) );

    mpMenuDialog->addControl( new GalaxyButton( "Controls",
                                    new OpenMenuEvent(	new CControlsettings() ) ) );
*/
}


}
