#include "cheatsmenu.h"


#include "engine/core/CBehaviorEngine.h"

namespace vorticon
{

CheatsMenu::CheatsMenu() :
VorticonMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f) )
{
    mpJumpSwitch = new Switch( "Fly Mode" );
    mpMenuDialog->addControl( mpJumpSwitch );

    mpGodSwitch = new Switch( "God Mode" );
    mpMenuDialog->addControl( mpGodSwitch );

    mpNoClipSwitch = new Switch( "No Clipping" );
    mpMenuDialog->addControl( mpNoClipSwitch );

    mpItemsSwitch = new Switch( "Free items" );
    mpMenuDialog->addControl( mpItemsSwitch );

    refresh();
}

void CheatsMenu::refresh()
{
    // TODO: More Cheats!!
    mpJumpSwitch->enable( gpBehaviorEngine->mCheatmode.jump );
    mpGodSwitch->enable( gpBehaviorEngine->mCheatmode.god );
    mpNoClipSwitch->enable( gpBehaviorEngine->mCheatmode.noclipping );
    mpItemsSwitch->enable( gpBehaviorEngine->mCheatmode.items );
}


void CheatsMenu::release()
{
    gpBehaviorEngine->mCheatmode.jump = mpJumpSwitch->isEnabled();
    gpBehaviorEngine->mCheatmode.god = mpGodSwitch->isEnabled();
    gpBehaviorEngine->mCheatmode.noclipping = mpNoClipSwitch->isEnabled();
    gpBehaviorEngine->mCheatmode.items = mpItemsSwitch->isEnabled();
}


}
