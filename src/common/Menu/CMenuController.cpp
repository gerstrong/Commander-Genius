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
#include "sdl/input/CInput.h"

void CMenuController::process()
{

	// process any triggered Game Control related event
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();


	// Did the player press the quit/back button
	if( g_pInput->getPressedCommand(IC_BACK) )
	{
		if( mMenuStack.empty() ) // If no menu is open, open the main menu
		{
			EventContainer.add( new OpenMenuEvent( new CMainMenu(DLG_THEME_VORTICON) ) );
		}
		else // Close the menu which is open. Might go back if it is a submenu
		{
			EventContainer.add( new CloseMenuEvent() );
		}
	}



	if(!EventContainer.empty())
	{

		if( OpenMenuEvent* openMenu = EventContainer.occurredEvent<OpenMenuEvent>() )
		{
			mpMenu = openMenu->mMenuDialogPointer;
			mpMenu->init();

			if( !mMenuStack.empty() )
				mpMenu->setProperty( CBaseMenu::CANGOBACK );

			mMenuStack.push_back( mpMenu );
			EventContainer.pop_Event();
		}

		if( EventContainer.occurredEvent<CloseMenuEvent>() )
		{
			mpMenu->release();
			mMenuStack.pop_back();
			mpMenu = mMenuStack.back();
			EventContainer.pop_Event();
		}

		if( OpenControlMenuEvent* ctrlMenu = EventContainer.occurredEvent<OpenControlMenuEvent>() )
		{
			EventContainer.pop_Event();

			EventContainer.add( new OpenMenuEvent(
									new CControlsettings(ctrlMenu->mDlgTheme,
														 ctrlMenu->mNumPlayers) ) );
		}


	}

	if( !mMenuStack.empty() )
		mpMenu->process();

}

