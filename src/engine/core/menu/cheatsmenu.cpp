#include "cheatsmenu.h"

#include "engine/core/CBehaviorEngine.h"


CheatsMenu::CheatsMenu(const GsControl::Style style) :
GameMenu(GsRect<float>(0.075f, 0.24f, 0.85f, 0.4f), style )
{
    mpJumpSwitch = new Switch( "Fly Mode", style );
    mpMenuDialog->addControl( mpJumpSwitch );

    mpGodSwitch = new Switch( "God Mode", style );
    mpMenuDialog->addControl( mpGodSwitch );

    mpNoClipSwitch = new Switch( "No Clipping", style );
    mpMenuDialog->addControl( mpNoClipSwitch );

    mpItemsSwitch = new Switch( "Free items", style );
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

