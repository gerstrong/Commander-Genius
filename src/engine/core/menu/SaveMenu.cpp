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

        mpMenuDialog->add( new InputText(
                                      text,
                                      GsRect<float>(
                                          0.0f, 0.1f+(i*0.1f),
                                          1.0f, 0.1f),
                                      style ) );

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
    auto &curWidget = mpMenuDialog->CurrentWidget();
    auto pInput = std::dynamic_pointer_cast<InputText>(curWidget);

    const auto noTyping = gBehaviorEngine.mOptions[GameOption::NOTYPING].value;

    if(noTyping)
    {
        GameMenu::ponder(deltaT);
    }
    else
    {
        int minIC = IC_LEFT;

        if(pInput!=nullptr)
        {
            if(pInput->Typing())
                minIC = IC_JUMP;
        }

        // Command (Keyboard/Joystick) are handled here
        for( int cmd = minIC ; cmd < MAX_COMMANDS ; cmd++ )
        {
            if( gInput.getPressedCommand(cmd) )
            {
                std::shared_ptr<CEvent> command(new CommandEvent( static_cast<InputCommand>(cmd) ));
                sendEvent(command);
                break;
            }
        }

        mpMenuDialog->processLogic();
    }

}


void CSaveMenu::sendEvent(std::shared_ptr<CEvent> &command)
{
    auto &curWidget = mpMenuDialog->CurrentWidget();
    auto pInput = std::dynamic_pointer_cast<InputText>(curWidget);

    const auto noTyping = gBehaviorEngine.mOptions[GameOption::NOTYPING].value;

	// Before all events are sent to the dialog which handles selection catch some specific events
	// required for the saving process.
	if( CommandEvent *ev = dynamic_cast<CommandEvent*>(command.get()) )
	{
		const int sel = mpMenuDialog->getSelection();
		if( sel > 0 )
		{            
			if(ev->mCommand == IC_JUMP || ev->mCommand == IC_STATUS)
            {
                if(noTyping)
                {
                    const std::string saveText = getTimeStr();

                    gSaveGameController.prepareSaveGame( sel, saveText );
                    gBehaviorEngine.setPause(false);
                    gEventManager.add( new CloseAllMenusEvent() );

                }
                else
                {
                    // we are typing...
                    if(pInput->Typing())
                    {
                        const std::string saveText = pInput->getText();
                        gSaveGameController.prepareSaveGame( sel, saveText );
                        gBehaviorEngine.setPause(false);
                        gEventManager.add( new CloseAllMenusEvent() );
                    }
                    else
                    {
                        if(pInput->getText() == EMPTY_TEXT)
                            pInput->setText("");
                        pInput->setTypeMode(true);
                    }
                }

				return;
			}
		}
	}

    if(pInput)
    {
        if(pInput->Typing())
            return;
    }


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
