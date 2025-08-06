#include "QuitMenu.h"

#include <base/CInput.h>
#include <base/GsApp.h>
#include <base/GsEventContainer.h>

#include "engine/core/mode/Scene.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/CGameLauncher.h"




QuitMenu::QuitMenu(const bool openedGamePlay,
                   const Style style) :
GameMenu( GsRect<float>(0.25f, 0.23f, 0.5f, 0.5f), style, false )
{
    auto endGameBtn =
         mpMenuDialog->add(
                new GameButton( "End Game",
                                new EventEndGamePlay(),
                                style ) );

    if(!openedGamePlay)
    {
        endGameBtn->enable(false);
    }

    auto backToLauncher = []()
    {
        gBehaviorEngine.setPause(false);
        gMenuController.clearMenuStack();
        gEventManager.add( new GMSwitchToGameLauncher() );
    };

    mpMenuDialog->add(
           new GameButton( "Back to Game Launcher",
                           backToLauncher,
                           style ) );

    mpMenuDialog->add(new GameButton( "Quit CG", new GMQuit(), style ) );

    setMenuLabel("MAINMENULABEL");

    mpMenuDialog->fit();
    select(0);
}


void QuitMenu::ponder(const float dt)
{
    // If IC_BACK is invoked, make the menu controller close the controller
    if( gInput.getPressedCommand(IC_BACK) )
    {
        gEventManager.add( new CloseMenuEvent(false) );
        return;
    }

    handleInput(dt);

    mpMenuDialog->processLogic();
}
