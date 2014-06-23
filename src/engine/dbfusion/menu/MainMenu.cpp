#include "MainMenu.h"

namespace dbfusion
{

MainMenu::MainMenu() : FusionMenu(GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f))
{
    GsButton *mpGameMenuButton = new GsButton( "Send Escape (Menu)", new BackButtonSendDosFusion());

    mpMenuDialog->addControl( mpGameMenuButton, GsRect<float>(0.2f, 0.2f, 0.6f, 0.05f) );
}


}
