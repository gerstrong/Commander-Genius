/*
 * CLoadMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CLoadMenu.h"
#include "fileio/CSaveGameController.h"
#include "common/CBehaviorEngine.h"
#include "gui/CGUIText.h"
#include "gui/CGUIButton.h"
#include "common/Menu/CMenuController.h"
#include "core/mode/CGameMode.h"


struct LoadGameSlotFunctorEvent : public InvokeFunctorEvent
{
	LoadGameSlotFunctorEvent(const Uint32 slot) : mSlot(slot) {}

	void operator()()
	{
		g_pBehaviorEngine->EventList().add( new CloseAllMenusEvent() );
		gpSaveGameController->prepareLoadGame(mSlot);
	}

	Uint32 mSlot;
};


CLoadMenu::CLoadMenu() :
CBaseMenu( CRect<float>(0.1f, 0.0f, 0.8f, 1.0f) )
{

	// Load the state-file list
	std::vector<std::string> StateFileList = gpSaveGameController->getSlotList();

	for( Uint32 i=1 ; i<=8 ; i++ )
	{
		std::string text = gpSaveGameController->getEmptyString();

		if( i <= StateFileList.size() )
		{
			text = StateFileList.at(i-1);
			mpMenuDialog->addControl( new CGUIButton( text,
														new LoadGameSlotFunctorEvent(i) ) );
		}
		else
		{

			mpMenuDialog->addControl( new CGUIText( text ) );
		}
	}

	setMenuLabel("LOADMENULABEL");
}
