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

#include "widgets/BorderedButton.h"

static const char *EMPTY_TEXT = "EMPTY";

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

        if(style == Style::GALAXY)
        {
            auto borderedButton =
            mpMenuDialog->add(
                new BorderedButton(text,
                                   GsRect<float>(
                                       0.0f, 0.1f+(i*0.1f),
                                       1.0f, 0.1f),
                                   new LoadGameSlotFunctorEvent(i),
                                   style) );

            borderedButton->enableCenteringH(true);
            borderedButton->enableBlinker(false);
            borderedButton->enable(false);
        }
        else if(style == Style::VORTICON)
        {
            auto textButton =
            mpMenuDialog->add(
                new GameButton(text,
                                   GsRect<float>(
                                       0.00f, 0.1f+(i*0.1f),
                                       1.00f, 0.1f),
                                   new LoadGameSlotFunctorEvent(i),
                                   style) );

            textButton->enableCenteringH(true);
            textButton->enableTwirl(true);
            textButton->enable(false);
        }
        else
        {
            auto textButton =
            mpMenuDialog->add(
                new GameButton(text,
                                   GsRect<float>(
                                       0.00f, 0.1f+(i*0.1f),
                                       1.00f, 0.1f),
                                   new LoadGameSlotFunctorEvent(i),
                                   style) );

            textButton->enableCenteringH(true);
            textButton->enableTwirl(gMenuController.mEnableTwirl);
            textButton->enable(false);
        }

    }

    setMenuLabel("LOADMENULABEL");
    select(1);
}


void CLoadMenu::sendEvent(std::shared_ptr<CEvent> &command)
{
    mpMenuDialog->sendEvent(command);
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

        const auto slotText = button->getText();

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
