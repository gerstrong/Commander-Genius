
#include "FusionMenu.h"


// Send the key used for the menu to the game.
// usually this ends up opening the ingame menu. (Esc Key Triggering)
struct BackButtonSendDosFusion : public CEvent {};


namespace dbfusion
{


class MainMenu : public FusionMenu
{

public:
    MainMenu() : FusionMenu(GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f))
    {
        GsButton *mpGameMenuButton = new GsButton( "DOS Game Menu", new BackButtonSendDosFusion());

        mpMenuDialog->addControl( mpGameMenuButton, GsRect<float>(0.2f, 0.2f, 0.6f, 0.05f) );
    }

};

}
