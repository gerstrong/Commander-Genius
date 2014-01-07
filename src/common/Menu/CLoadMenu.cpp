/*
 * CLoadMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CLoadMenu.h"
#include "common/CBehaviorEngine.h"
#include "gui/CGUIText.h"
#include "gui/CGUIButton.h"
#include <widgets/CMenuController.h>
#include "fileio/CSaveGameController.h"
#include "common/CBehaviorEngine.h"
#include "core/mode/CGameMode.h"


struct LoadGameSlotFunctorEvent : public InvokeFunctorEvent
{
	LoadGameSlotFunctorEvent(const Uint32 slot) : mSlot(slot) {}

    void operator()() const
	{
		gEventManager.add( new CloseAllMenusEvent() );
		gpSaveGameController->prepareLoadGame(mSlot);
	}

	Uint32 mSlot;
};


CLoadMenu::CLoadMenu() :
CBaseMenu( GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f) )
{
	// Load the state-file list
	const CGUIControl::Style style =
			( g_pBehaviorEngine->getEngine() == ENGINE_GALAXY ) ?
					CGUIButton::GALAXY_BORDERED : CGUIButton::VORTICON;
	std::vector<std::string> StateFileList = gpSaveGameController->getSlotList();
	std::vector<std::string>::iterator it = StateFileList.begin();

	Uint32 i=1;
	CGUIButton *button;
	std::string text;

	for( ; it != StateFileList.end() && i<8 ; i++,it++ )
	{
		text = *it;

		if( !text.empty() )
		{
			button = new CGUIButton( text,
									new LoadGameSlotFunctorEvent(i),
									style );
			mpMenuDialog->addControl( button );

			button->enable( true );
		}
	}

	for( int j = i ; j<8 ; j++ )
	{
		text = "Empty";
		button = new CGUIButton( text,
								new LoadGameSlotFunctorEvent(j),
								style );
		mpMenuDialog->addControl( button );

		button->enable( false );
	}

	setMenuLabel("LOADMENULABEL");
}
