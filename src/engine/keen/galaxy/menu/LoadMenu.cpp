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
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/mode/CGameMode.h"

#include "widgets/BorderedButton.h"


namespace galaxy
{


struct LoadGameSlotFunctorEvent : public InvokeFunctorEvent
{
    LoadGameSlotFunctorEvent(const Uint32 slot) : mSlot(slot+1) {}

    void operator()() const
	{
        g_pBehaviorEngine->setPause(false);
		gEventManager.add( new CloseAllMenusEvent() );
		gpSaveGameController->prepareLoadGame(mSlot);
	}

	Uint32 mSlot;
};


CLoadMenu::CLoadMenu() :
GalaxyMenu( GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f) )
{
    for( int j = 0 ; j<8 ; j++ )
	{
        GsButton *button = new BorderedButton( "Empty",
                                new LoadGameSlotFunctorEvent(j));
        mpMenuDialog->addControl( button,
                                  GsRect<float>(
                                      0.0f, 0.1f+(j*0.1f), 0.8f, 0.1f) );

		button->enable( false );
	}

	setMenuLabel("LOADMENULABEL");
}



void CLoadMenu::refresh()
{
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gpSaveGameController->readSlotList(StateFileList);

    std::vector<std::string>::iterator it = StateFileList.begin();

    Uint32 i=0;

    std::list< std::shared_ptr<CGUIControl> > &list = mpMenuDialog->getControlList();

    auto itCtrl = list.begin();
    itCtrl++;

    for( ; it != StateFileList.end() && i<8 ; i++, it++ )
    {
        const std::string text = *it;

        if( !text.empty() )
        {
            std::shared_ptr<CGUIControl> &ctrl = *itCtrl;
            GsButton *button = dynamic_cast<GsButton*>( ctrl.get() );

            button->setText(text);
            button->enable( true );
        }

        itCtrl++;
    }

    for( int j = i ; j<8 ; j++ )
    {
        std::shared_ptr<CGUIControl> &ctrl = *itCtrl;
        GsButton *button = dynamic_cast<GsButton*>( ctrl.get() );

        button->setText("Empty");
        button->enable( false );
        itCtrl++;
    }

}



}
