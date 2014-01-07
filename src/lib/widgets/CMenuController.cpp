/*
 * CMenuController.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong
 */

#include "CMenuController.h"
#include "common/CBehaviorEngine.h"
#include "common/Menu/CControlsettings.h"
#include "common/Menu/CMainMenu.h"
#include <base/CInput.h>
#include "sdl/music/CMusic.h"


void CMenuController::emptyMenuStack()
{
	while(!mMenuStack.empty())
		popBackMenu();
}


void CMenuController::openMainMenu()
{
	if(mLocked)
	    return;

    gEventManager.add( new OpenMenuEvent( new CMainMenu(mOpenedGamePlay) ) );
	g_pBehaviorEngine->setPause(true);
	g_pMusicPlayer->pause();	
	
	// When menu is opened show the cursor
	SDL_ShowCursor(SDL_ENABLE);
}


void CMenuController::pumpEvent(const CEvent *evPtr)
{
    CEventContainer &EventContainer = gEventManager;

    if( const OpenMenuEvent* openMenu = dynamic_cast<const OpenMenuEvent*>(evPtr) )
    {
        CBaseMenu &menu = *openMenu->mMenuDialogPointer.get();
        menu.init();

        // Select the second element. The first one (0) is the close button.
        menu.select(1);

        if( !mMenuStack.empty() )
        menu.setProperty( CBaseMenu::CANGOBACK );

        mMenuStack.push_back( openMenu->mMenuDialogPointer );
    }

    if( dynamic_cast<const CloseMenuEvent*>(evPtr) )
    {
        popBackMenu();

        if(mMenuStack.empty())
            g_pMusicPlayer->play();
    }

    if( dynamic_cast<const CloseAllMenusEvent*>(evPtr) )
    {
        emptyMenuStack();

        g_pMusicPlayer->play();
    }

    // Control Menu Events
    if( const OpenMovementControlMenuEvent* ctrlMenu = dynamic_cast<const OpenMovementControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        EventContainer.add( new OpenMenuEvent(
                                new CControlSettingsMovement(players) ) );
    }

    if( const OpenButtonsControlMenuEvent* ctrlMenu = dynamic_cast<const OpenButtonsControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        EventContainer.add( new OpenMenuEvent(
                                new CControlSettingsButtons(players) ) );
    }

    if( const OpenControlMenuEvent* ctrlMenu = dynamic_cast<const OpenControlMenuEvent*>(evPtr) )
    {
        const int players = ctrlMenu->mSelection;
        EventContainer.add( new OpenMenuEvent(
                                new CControlsettings(players) ) );
    }
}


void CMenuController::ponder(const float deltaT)
{
	if(mLocked)
	    return;
    
    CEventContainer &EventContainer = gEventManager;


	// Did the player press the quit/back button
	if( gInput.getPressedCommand(IC_BACK) )
	{
		if( mMenuStack.empty() ) // If no menu is open, open the main menu
		{
			openMainMenu();
		}
		else // Close the menu which is open. Might go back if it is a submenu
		{
			EventContainer.add( new CloseMenuEvent() );
		}
	}


    // Process Menu if open
	if( !mMenuStack.empty() )
	{
        mMenuStack.back()->ponder(deltaT);
	}
}

void CMenuController::popBackMenu()
{
    mMenuStack.back()->release();
    mMenuStack.pop_back();
    
    if(mMenuStack.empty())
    {
        // The last menu has been removed. Restore back the game status
        g_pBehaviorEngine->setPause(false);
		
        // When menu is closed hide the cursor
        SDL_ShowCursor(SDL_DISABLE);
    }
}


void CMenuController::render()
{
    // Render the Menu if open
    if( !mMenuStack.empty() && !mHidden )
    {
        mMenuStack.back()->render();
    }
}


void CMenuController::updateGraphics()
{
    for( auto &menu : mMenuStack)
    {
        menu->updateGraphics();
    }
}
