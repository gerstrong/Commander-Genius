/*
 * CMenuController.cpp
 *
 *  Created on: 19.02.2012
 *      Author: gerstrong
 */

#include "GsMenuController.h"
#include <base/CInput.h>
#include <cassert>

void CMenuController::clearMenuStack()
{
	while(!mMenuStack.empty())
		popBackMenu();
}


void CMenuController::pumpEvent(const CEvent *evPtr)
{
    if( const OpenMenuEvent* openMenu = dynamic_cast<const OpenMenuEvent*>(evPtr) )
    {
        CBaseMenu &menu = *openMenu->mMenuDialogPointer.get();
        menu.refresh();

        // Select the second element. The first one (0) is the close button.
        menu.select(1);

        if( !mMenuStack.empty() )
            menu.setProperty( CBaseMenu::CANGOBACK );

        mMenuStack.push_back( openMenu->mMenuDialogPointer );
    }
    else if( dynamic_cast<const CloseMenuEvent*>(evPtr) )
    {
        popBackMenu();
    }
    else if( dynamic_cast<const CloseAllMenusEvent*>(evPtr) )
    {
        clearMenuStack();
    }
}


void CMenuController::ponder(const float deltaT)
{
    // Process Menu if open
	if( !mMenuStack.empty() )
	{
        mMenuStack.back()->ponder(deltaT);
	}
}

void CMenuController::popBackMenu()
{
    // This should not be called when menu stack is empty
    assert(!mMenuStack.empty());

    mMenuStack.back()->release();
    mMenuStack.pop_back();
    
    if(mMenuStack.empty())
    {		
        // When menu is closed hide the cursor
        //SDL_ShowCursor(SDL_DISABLE);
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
