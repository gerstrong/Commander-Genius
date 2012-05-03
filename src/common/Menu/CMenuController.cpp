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
#include "sdl/music/CMusic.h"


void CMenuController::process()
{

	// process any triggered Game Control related event
	CEventContainer &EventContainer = g_pBehaviorEngine->EventList();


	// Did the player press the quit/back button
	if( g_pInput->getPressedCommand(IC_BACK) )
	{
		if( mMenuStack.empty() ) // If no menu is open, open the main menu
		{
			EventContainer.add( new OpenMenuEvent( new CMainMenu(mOpenedGamePlay) ) );
			g_pBehaviorEngine->setPause(true);
			g_pMusicPlayer->pause();
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
			popBackMenu();
			EventContainer.pop_Event();

			if(EventContainer.empty())
				g_pMusicPlayer->play();
		}

		if( EventContainer.occurredEvent<CloseAllMenusEvent>() )
		{
			while(!mMenuStack.empty())
				popBackMenu();

			EventContainer.pop_Event();
			g_pMusicPlayer->play();
		}

		// Control Menu Events
		if( OpenMovementControlMenuEvent* ctrlMenu = EventContainer.occurredEvent<OpenMovementControlMenuEvent>() )
		{
			const int players = ctrlMenu->mNumPlayers;
			EventContainer.pop_Event();
			EventContainer.add( new OpenMenuEvent(
									new CControlSettingsMovement(players) ) );
		}

		if( OpenButtonsControlMenuEvent* ctrlMenu = EventContainer.occurredEvent<OpenButtonsControlMenuEvent>() )
		{
			const int players = ctrlMenu->mNumPlayers;
			EventContainer.pop_Event();
			EventContainer.add( new OpenMenuEvent(
									new CControlSettingsButtons(players) ) );
		}

		if( OpenControlMenuEvent* ctrlMenu = EventContainer.occurredEvent<OpenControlMenuEvent>() )
		{
			const int players = ctrlMenu->mNumPlayers;
			EventContainer.pop_Event();
			EventContainer.add( new OpenMenuEvent(
									new CControlsettings(players) ) );
		}


	}


	if( !mMenuStack.empty() )
	{
		mpMenu->process();
	}

}

void CMenuController::popBackMenu()
{
	mpMenu->release();
	mMenuStack.pop_back();

	if(!mMenuStack.empty())
	{
		mpMenu = mMenuStack.back();
	}
	else
	{
		g_pBehaviorEngine->setPause(false);
		mpMenu = NULL;
	}
}


