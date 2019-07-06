/*
 *
 * CLoadMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */


#include "LoadMenu.h"

#include <base/CInput.h>
#include <base/GsEventContainer.h>
#include <widgets/GsButton.h>
#include <widgets/GsMenuController.h>

#include "fileio/CSaveGameController.h"
#include "engine/core/CBehaviorEngine.h"
#include "engine/core/mode/CGameMode.h"

#include "widgets/InputText.h"

#include "widgets/BorderedButton.h"


const std::string EMPTY_TEXT = "EMPTY";

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
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    for(Uint32 i=0 ; i<8 ; i++)
    {
        std::string text = EMPTY_TEXT;
        if(i < StateFileList.size())
            text = StateFileList.at(i);

        auto inputText =
            mpMenuDialog->add( new InputText(text,
                                             GsRect<float>(
                                                   0.0f, 0.1f+(i*0.1f),
                                                   1.0f, 0.1f),style ) );

        std::shared_ptr<LoadGameSlotFunctorEvent>
                loadGameEv(new LoadGameSlotFunctorEvent(i)) ;

        auto ev = std::static_pointer_cast<CEvent>(loadGameEv);
        inputText->setEvent(ev);
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
