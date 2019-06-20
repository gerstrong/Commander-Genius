/*
 *
 * CLoadMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

//#include <widgets/GsText.h>
#include <widgets/GsButton.h>
#include <widgets/GsMenuController.h>

#include "LoadMenu.h"
#include <base/GsEventContainer.h>

#include "fileio/CSaveGameController.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/mode/CGameMode.h"

#include "widgets/BorderedButton.h"




struct LoadGameSlotFunctorEvent : public InvokeFunctorEvent
{
    LoadGameSlotFunctorEvent(const Uint32 slot) : mSlot(slot+1) {}

    void operator()() const
	{
        gBehaviorEngine.setPause(false);
		gEventManager.add( new CloseAllMenusEvent() );
		gSaveGameController.prepareLoadGame(mSlot);
	}

	Uint32 mSlot;
};


CLoadMenu::CLoadMenu(const Style &style) :
GameMenu( GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f),
          style)
{
    for( int j = 0 ; j<8 ; j++ )
	{
        std::shared_ptr<GsButton> button;

        if(style == Style::GALAXY)
        {
            button.reset(new BorderedButton( "Empty",
                                         GsRect<float>(
                                             0.0f, 0.1f+(j*0.1f), 0.7f, 0.1f),
                                         new LoadGameSlotFunctorEvent(Uint32(j)),
                                         style));
            mpMenuDialog->addWidget(button);
        }
        else
        {
            button.reset(new GameButton( "Empty",
                                     new LoadGameSlotFunctorEvent(Uint32(j)),
                                     style) );

        }        
        mpMenuDialog->addWidget( button );

        button->enable( false );
	}

	setMenuLabel("LOADMENULABEL");
}



void CLoadMenu::refresh()
{
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    std::vector<std::string>::iterator it = StateFileList.begin();

    Uint32 i=0;

    auto &list = mpMenuDialog->getWidgetList();

    auto itCtrl = list.begin();
    itCtrl++;

    for( ; it != StateFileList.end() && i<8 ; i++, it++ )
    {
        const std::string text = *it;

        if( !text.empty() )
        {
            auto &ctrl = *itCtrl;
            GsButton *button = dynamic_cast<GsButton*>( ctrl.get() );

            button->setText(text);
            button->enable( true );
        }

        itCtrl++;
    }

    for( auto j = i ; j<8 ; j++ )
    {
        auto &ctrl = *itCtrl;
        GsButton *button = dynamic_cast<GsButton*>( ctrl.get() );

        button->setText("Empty");
        button->enable( false );
        itCtrl++;
    }

}
