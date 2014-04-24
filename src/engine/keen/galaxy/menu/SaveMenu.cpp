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

#include "CConfirmMenu.h"

#include "fileio/CSaveGameController.h"
#include "engine/core/CBehaviorEngine.h"

#include "widgets/InputText.h"

const std::string EMPTY_TEXT = "EMPTY";

namespace galaxy
{

CSaveMenu::CSaveMenu() :
GalaxyMenu(GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f) ),
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

        mpMenuDialog->addControl(new InputText( text ) );
	}

	setMenuLabel("SAVEMENULABEL");
}

void CSaveMenu::ponder()
{
    InputText *pInput = dynamic_cast<InputText*>(mpMenuDialog->CurrentControl());

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
    InputText *pInput = dynamic_cast<InputText*>(mpMenuDialog->CurrentControl());

	// Before all events are sent to the dialog which handles selection catch some specific events
	// required for the saving process.
	if( CommandEvent *ev = dynamic_cast<CommandEvent*>(command.get()) )
	{
		const int sel = mpMenuDialog->Selection();
		if( sel > 0 )
		{            
			if(ev->mCommand == IC_JUMP || ev->mCommand == IC_STATUS)
            {
                // we are typing...
				if(pInput->Typing())
				{
					gpSaveGameController->prepareSaveGame( sel, pInput->getText() );
                    g_pBehaviorEngine->setPause(false);
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

    if(pInput)
    {
        if(pInput->Typing())
            return;
    }


	mpMenuDialog->sendEvent(command);
}

void CSaveMenu::release()
{
    InputText *pInput = dynamic_cast<InputText*>(mpMenuDialog->CurrentControl());

    if(pInput)
    {
        pInput->setTypeMode(false);
    }
}

}
