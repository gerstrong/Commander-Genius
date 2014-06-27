#include "MainMenu.h"

#include <base/GsEvent.h>


namespace dbfusion
{

int mapperRun(void*)
{
    MAPPER_RunInternal();
}


MainMenu::MainMenu() : FusionMenu(GsRect<float>(0.0f, 0.0f, 1.0f, 1.0f))
{
    // Button for sending back the menu button
    GsButton *pGameMenuButton = new GsButton( "Send Escape (Menu)", new BackButtonSendDosFusion());
    mpMenuDialog->addControl( pGameMenuButton, GsRect<float>(0.2f, 0.2f, 0.6f, 0.05f) );

    GsButton *pMapperButton = new GsButton( "Mapper", new ExecuteMappperEvent);
    mpMenuDialog->addControl( pMapperButton, GsRect<float>(0.2f, 0.3f, 0.6f, 0.05f) );

}


}
