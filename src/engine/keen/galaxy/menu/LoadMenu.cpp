/*
 *
 * CLoadMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include <widgets/GsText.h>
#include <widgets/GsButton.h>
#include <widgets/GsMenuController.h>

#include "LoadMenu.h"

#include "fileio/CSaveGameController.h"
#include "common/CBehaviorEngine.h"
#include "common/CBehaviorEngine.h"
#include "core/mode/CGameMode.h"

#include "widgets/BorderedButton.h"


namespace galaxy
{


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
GalaxyMenu( GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f) )
{
	// Load the state-file list
	std::vector<std::string> StateFileList = gpSaveGameController->getSlotList();
	std::vector<std::string>::iterator it = StateFileList.begin();

	Uint32 i=1;
	GsButton *button;
	std::string text;

	for( ; it != StateFileList.end() && i<8 ; i++,it++ )
	{
		text = *it;

		if( !text.empty() )
		{
            button = new BorderedButton( text,
                                    new LoadGameSlotFunctorEvent(i) );
			mpMenuDialog->addControl( button );

			button->enable( true );
		}
	}

	for( int j = i ; j<8 ; j++ )
	{
		text = "Empty";
        button = new BorderedButton( text,
                                new LoadGameSlotFunctorEvent(j));
		mpMenuDialog->addControl( button );

		button->enable( false );
	}

	setMenuLabel("LOADMENULABEL");
}

}
