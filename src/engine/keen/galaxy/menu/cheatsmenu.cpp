#include "cheatsmenu.h"

#include "engine/core/CBehaviorEngine.h"

namespace galaxy
{

CheatsMenu::CheatsMenu() :
GalaxyMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f) )
{
    mpJumpSwitch = new Switch( "Fly Mode" );
    mpMenuDialog->addControl( mpJumpSwitch );

    mpGodSwitch = new Switch( "God Mode" );
    mpMenuDialog->addControl( mpGodSwitch );

    mpNoClipSwitch = new Switch( "No Clipping" );
    mpMenuDialog->addControl( mpNoClipSwitch );

    mpItemsSwitch = new Switch( "Free items" );
    mpMenuDialog->addControl( mpItemsSwitch );

}

void CheatsMenu::refresh()
{
    // TODO: More Cheats!!    
    mpJumpSwitch->enable( gBehaviorEngine.mCheatmode.jump );
    mpGodSwitch->enable( gBehaviorEngine.mCheatmode.god );
    mpNoClipSwitch->enable( gBehaviorEngine.mCheatmode.noclipping );
    mpItemsSwitch->enable( gBehaviorEngine.mCheatmode.items );
}


void CheatsMenu::release()
{
    gBehaviorEngine.mCheatmode.jump = mpJumpSwitch->isEnabled();
    gBehaviorEngine.mCheatmode.god = mpGodSwitch->isEnabled();
    gBehaviorEngine.mCheatmode.noclipping = mpNoClipSwitch->isEnabled();
    gBehaviorEngine.mCheatmode.items = mpItemsSwitch->isEnabled();
}


}
