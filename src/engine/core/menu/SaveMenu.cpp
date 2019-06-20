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

const std::string EMPTY_TEXT = "EMPTY";


CSaveMenu::CSaveMenu(const GsControl::Style &style) :
GameMenu(GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f), style )
{
	// Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    for(Uint32 i=0 ; i<8 ; i++)
	{
		std::string text = EMPTY_TEXT;
        if(i < StateFileList.size())
            text = StateFileList.at(i);

        mpMenuDialog->addWidget( new InputText(
                                      text,
                                      GsRect<float>(
                                          0.0f, 0.1f+(i*0.1f), 0.7f, 0.1f),
                                      style ) );

	}

	setMenuLabel("SAVEMENULABEL");

    //mpMenuDialog->fit();
}


void CSaveMenu::refresh()
{
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gSaveGameController.readSlotList(StateFileList);

    auto &list =
            mpMenuDialog->getWidgetList();

    auto itCtrl = list.begin();
    itCtrl++;

    for(Uint32 i=0 ; i<8 ; i++)
    {
        std::string text = EMPTY_TEXT;

        if(i < StateFileList.size())
            text = StateFileList.at(i);

        auto &ctrl = *itCtrl;
        InputText *input = dynamic_cast<InputText*>( ctrl.get() );

        input->setText(text);
        itCtrl++;
    }
}


void CSaveMenu::ponder(const float)
{
    auto &curWidget = mpMenuDialog->CurrentWidget();
    auto pInput = std::dynamic_pointer_cast<InputText>(curWidget);

    int minIC = IC_LEFT;

    if(pInput!=nullptr)
    {
        if(pInput->Typing())
            minIC = IC_JUMP;
    }

#if !defined(NOTYPESAVE)
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
#else

    auto &list =
            mpMenuDialog->getWidgetList();

    auto itCtrl = list.begin();
    itCtrl++;

    for(int i=0 ; i<8 ; i++)
    {
        auto &ctrl = *itCtrl;
        InputText *input = dynamic_cast<InputText*>( ctrl.get() );

        if(input->isPressed())
        {
            mpMenuDialog->setSelection(i+1);

            std::shared_ptr<CEvent> command(
                        new CommandEvent( static_cast<InputCommand>(IC_JUMP) ));
            sendEvent(command);
            break;
        }

        itCtrl++;
    }

#endif

    mpMenuDialog->processLogic();

}

void CSaveMenu::sendEvent(std::shared_ptr<CEvent> &command)
{
    auto &curWidget = mpMenuDialog->CurrentWidget();
    auto pInput = std::dynamic_pointer_cast<InputText>(curWidget);

	// Before all events are sent to the dialog which handles selection catch some specific events
	// required for the saving process.
	if( CommandEvent *ev = dynamic_cast<CommandEvent*>(command.get()) )
	{
		const int sel = mpMenuDialog->Selection();
		if( sel > 0 )
		{            
			if(ev->mCommand == IC_JUMP || ev->mCommand == IC_STATUS)
            {
#ifdef NOTYPESAVE
                const std::string saveText = getTimeStr();

#else
                // we are typing...
				if(pInput->Typing())
				{
                    const std::string saveText = pInput->getText();
#endif
                    gSaveGameController.prepareSaveGame( sel, saveText );
                    gBehaviorEngine.setPause(false);
					gEventManager.add( new CloseAllMenusEvent() );
#ifndef NOTYPESAVE
				}
				else
				{
					if(pInput->getText() == EMPTY_TEXT)
						pInput->setText("");
					pInput->setTypeMode(true);
				}
#endif
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
