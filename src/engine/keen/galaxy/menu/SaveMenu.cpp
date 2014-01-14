/*
 * CSaveMenu.cpp
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#include "SaveMenu.h"

#include <base/CInput.h>
#include <widgets/GsBaseMenu.h>
#include <widgets/GsInputText.h>
#include <widgets/GsMenuController.h>

#include "CConfirmMenu.h"

#include "fileio/CSaveGameController.h"
#include "common/CBehaviorEngine.h"

const std::string EMPTY_TEXT = "EMPTY";

CSaveMenu::CSaveMenu() :
CBaseMenu(GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f) ),
mp_OverwriteMenu(NULL),
m_overwrite(false)
{
	// Load the state-file list
	std::vector<std::string> StateFileList = gpSaveGameController->getSlotList();

	for(Uint32 i=1;i<=8;i++)
	{
		std::string text = EMPTY_TEXT;
		if(i <= StateFileList.size())
			text = StateFileList.at(i-1);

		mpMenuDialog->addControl(new CGUIInputText( text, CGUIInputText::VORTICON ) );
	}

	setMenuLabel("SAVEMENULABEL");
}

void CSaveMenu::ponder()
{
    CGUIInputText *pInput = dynamic_cast<CGUIInputText*>(mpMenuDialog->CurrentControl());

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
            std::shared_ptr<CEvent> command(new CommandEvent( static_cast<InputCommands>(cmd) ));
            sendEvent(command);
            break;
        }
    }

    mpMenuDialog->processLogic();

}

void CSaveMenu::sendEvent(std::shared_ptr<CEvent> &command)
{
	// Before all events are sent to the dialog which handles selection catch some specific events
	// required for the saving process.
	if( CommandEvent *ev = dynamic_cast<CommandEvent*>(command.get()) )
	{
		const int sel = mpMenuDialog->Selection();
		if( sel > 0 )
		{
            CGUIInputText *pInput = dynamic_cast<CGUIInputText*>(mpMenuDialog->CurrentControl());

			if(ev->mCommand == IC_JUMP || ev->mCommand == IC_STATUS)
			{			
				if(pInput->Typing())
				{
					gpSaveGameController->prepareSaveGame( sel, pInput->getText() );
					gEventManager.add( new CloseAllMenusEvent() );
				}
				else
				{
					if(pInput->getText() == EMPTY_TEXT)
						pInput->setText("");
					pInput->setTypeMode(true);
				}
				return;
			}
		}
	}

	mpMenuDialog->sendEvent(command);
}

void CSaveMenu::release()
{
    CGUIInputText *pInput = dynamic_cast<CGUIInputText*>(mpMenuDialog->CurrentControl());

    if(pInput)
    {
        pInput->setTypeMode(false);
    }
}
