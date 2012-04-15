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
		g_pBehaviorEngine->EventList().add( new StartGameplayEvent() );
		gpSaveGameController->prepareLoadGame(mSlot);

	}

	Uint32 mSlot;
};


CLoadMenu::CLoadMenu(Uint8 dlg_theme) :
CBaseMenu(dlg_theme, CRect<float>(0.1f, 0.0f, 0.8f, 1.0f) )
{

	mpMenuDialog->setBackground( CGUIDialog::VORTICON );

	// Load the state-file list
	std::vector<std::string> StateFileList = gpSaveGameController->getSlotList();

	for( Uint32 i=1 ; i<=20 ; i++ )
	{
		std::string text = gpSaveGameController->getEmptyString();

		if( i <= StateFileList.size() )
		{
			text = StateFileList.at(i-1);
			mpMenuDialog->addControl( new CGUIButton( text,
														new LoadGameSlotFunctorEvent(i),
														CGUIButton::VORTICON ) );
		}
		else
		{

			mpMenuDialog->addControl( new CGUIText( text ) );
		}
	}
}
