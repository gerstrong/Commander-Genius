/*
 * CSaveMenu.cpp
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#include "SaveMenu.h"

#include <base/CInput.h>
#include <widgets/GsBaseMenu.h>
#include <widgets/GsMenuController.h>


#include "fileio/CSaveGameController.h"
#include "engine/core/CBehaviorEngine.h"

#include "widgets/InputText.h"

#include <base/utils/misc.h>

static const std::string EMPTY_TEXT = "EMPTY";


CSaveMenu::CSaveMenu(const Style &style) :
GameMenu(GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f), style )
{
	// Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    for(int i=0 ; i<int(gSaveGameController.getMaxNumofSaveSlots()) ; i++)
	{
		std::string text = EMPTY_TEXT;
        if(i < int(StateFileList.size()))
            text = StateFileList.at(i);

        auto input =
        mpMenuDialog->add( new InputText(
                                      text,
                                      GsRect<float>(
                                          0.0f, 0.1f+(i*0.1f),
                                          1.0f, 0.1f), i,
                                      style ) );

        input->setActivationEvent([input]()
        {
            const auto noTyping = gBehaviorEngine.mOptions[GameOption::NOTYPING].value;
            const auto index = input->getIndex();

            if(noTyping)
            {
                const std::string saveText = getTimeStr();
                gSaveGameController.prepareSaveGame( index, saveText );
                gBehaviorEngine.setPause(false);
                gEventManager.add( new CloseAllMenusEvent() );
            }
            else // If typing option is enabled
            {
                if(!input->Typing())
                { // Not yet typing, so enable it
                    input->setTypeMode(true);
                    const std::string saveText = input->getText();
                    if(saveText == EMPTY_TEXT)
                        input->setText("");
                }
                else
                { // Here we finished typing. Let's save what user has written
                    const std::string saveText = input->getText();
                    gSaveGameController.prepareSaveGame( index, saveText );
                    gBehaviorEngine.setPause(false);
                    gEventManager.add( new CloseAllMenusEvent() );
                }
            }

            input->setReleased(false);
        });
	}

	setMenuLabel("SAVEMENULABEL");
    select(1);
}


void CSaveMenu::refresh()
{
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    std::vector<std::string>::iterator it = StateFileList.begin();

    auto &list = mpMenuDialog->getWidgetList();
    auto itCtrl = list.begin();
    itCtrl++;

    for( auto i = 0 ;
         it != StateFileList.end() && i<gSaveGameController.getMaxNumofSaveSlots() ;
         i++ )
    {
        const std::string text = *it;

        auto button =
                std::static_pointer_cast<GsButton>(*itCtrl);

        button->setText(EMPTY_TEXT);
        button->enable(true);

        if( !text.empty() )
        {
            button->setText(text);
            button->enable(true);
        }

        itCtrl++;
        it++;
    }
}


void CSaveMenu::ponder(const float deltaT)
{
    (void) deltaT;

    auto &curWidget = mpMenuDialog->CurrentWidget();
    auto pInput = std::dynamic_pointer_cast<InputText>(curWidget);

    mpMenuDialog->processLogic();


    auto &list = mpMenuDialog->getWidgetList();
    auto itTyping = list.begin();

    int typingIndex = -1;

    for(  ; itTyping != list.end() ; itTyping++ )
    {
        auto input =
                dynamic_cast<InputText*>(itTyping->get());

        if(!input)
            continue;

        if(input->Typing())
           typingIndex = input->getIndex();
    }

    // One of inputs is in typing mode
    for(auto it=list.begin()  ; it != list.end() ; it++ )
    {
        auto input =
                dynamic_cast<InputText*>(it->get());

        if(!input)
            continue;

        if(typingIndex < 0)
            input->enable(true);
        else if(typingIndex == input->getIndex())
            input->enable(true);
        else
            input->enable(false);
    }
}


void CSaveMenu::sendEvent(std::shared_ptr<CEvent> &command)
{
	mpMenuDialog->sendEvent(command);
}

void CSaveMenu::release()
{
    auto &curWidget = mpMenuDialog->CurrentWidget();
    auto pInput = std::dynamic_pointer_cast<InputText>(curWidget);

    if(pInput)
    {
        pInput->setTypeMode(false);
    }
}
