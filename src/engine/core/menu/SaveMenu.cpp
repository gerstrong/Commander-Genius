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
#include <base/audio/music/CMusicPlayer.h>

#include <string>

static const char *EMPTY_TEXT = "EMPTY";


CSaveMenu::CSaveMenu(const Style &style) :
GameMenu(GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f), style )
{
	// Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    for(int i=0 ; i<int(gSaveGameController.getMaxNumofSaveSlots()) ; i++)
	{
        std::string text(EMPTY_TEXT);
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

            // The naming of the files start with "1",
            // meaning "1" is the first element, not 0.
            const auto index = input->getIndex()+1;

            if(noTyping)
            {
                const std::string saveText = getTimeStr();
                gSaveGameController.prepareSaveGame( index, saveText );
                gBehaviorEngine.setPause(false);
                gEventManager.add( new CloseAllMenusEvent() );
                gEventManager.add( new EventReloadMusic );
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
                    gEventManager.add(new EventReloadMusic);
                }
            }

            input->setReleased(false);
        });
	}

	setMenuLabel("SAVEMENULABEL");
    select(1);
}


void CSaveMenu::sendEvent(std::shared_ptr<CEvent> &command)
{
    if(mIsTyping)
        return;

    mpMenuDialog->sendEvent(command);
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

    int i = 0;

    const auto maxNumSlots = gSaveGameController.getMaxNumofSaveSlots();

    for( ; it != StateFileList.end() &&
           i<maxNumSlots ; i++ )
    {
        const std::string text = *it;

        auto button =
                std::static_pointer_cast<GsButton>(*itCtrl);

        button->setText(std::string(EMPTY_TEXT));
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
    GameMenu::ponder(deltaT);

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

    mIsTyping = (typingIndex != -1);

    // Here we ensure that if in typing mode all the other controls
    // are disabled and if not all are enabled
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



void CSaveMenu::release()
{
    auto &curWidget = mpMenuDialog->CurrentWidget();
    auto pInput = std::dynamic_pointer_cast<InputText>(curWidget);

    if(pInput)
    {
        pInput->setTypeMode(false);
    }
}
