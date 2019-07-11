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
    LoadGameSlotFunctorEvent(const int slot) : mSlot(slot+1) {}

    void operator()() const
	{
        gBehaviorEngine.setPause(false);
		gEventManager.add( new CloseAllMenusEvent() );
		gSaveGameController.prepareLoadGame(mSlot);
	}

    int mSlot;
};


CLoadMenu::CLoadMenu(const Style &style) :
GameMenu( GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f),
          style)
{
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    for(int i=0 ; i<8 ; i++)
    {
        std::string text = EMPTY_TEXT;
        if(i < int(StateFileList.size()))
            text = StateFileList.at(int(i));

        auto inputText =
            mpMenuDialog->add( new InputText(text,
                                             GsRect<float>(
                                                   0.0f, 0.1f+(i*0.1f),
                                                   1.0f, 0.1f),style ) );

        std::shared_ptr<LoadGameSlotFunctorEvent>
                loadGameEv(new LoadGameSlotFunctorEvent(i)) ;

        auto ev = std::static_pointer_cast<CEvent>(loadGameEv);
        inputText->setEvent(ev);
        inputText->enable(false);
    }

	setMenuLabel("LOADMENULABEL");
    select(1);
}



void CLoadMenu::refresh()
{
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    std::vector<std::string>::iterator it = StateFileList.begin();

    auto &list = mpMenuDialog->getWidgetList();
    auto itCtrl = list.begin();
    itCtrl++;

    for( auto i = 0 ;
         it != StateFileList.end() && i<8 ;
         i++ )
    {
        const std::string text = *it;

        auto button =
                std::static_pointer_cast<GsButton>(*itCtrl);

        button->setText(EMPTY_TEXT);
        button->enable(false);

        if( !text.empty() )
        {
            button->setText(text);
            button->enable(true);
        }

        itCtrl++;
        it++;
    }

}
