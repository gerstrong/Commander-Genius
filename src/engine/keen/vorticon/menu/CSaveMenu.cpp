/*
 * CSaveMenu.cpp
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#include "CSaveMenu.h"

#include <base/CInput.h>
#include <widgets/GsBaseMenu.h>
#include <widgets/GsMenuController.h>
#include <base/utils/misc.h>


#include "widgets/Inputtext.h"
#include "fileio/CSaveGameController.h"
#include "engine/core/CBehaviorEngine.h"

#include <iostream>
#include <iomanip>
#include <ctime>

const std::string EMPTY_TEXT = "EMPTY";

namespace vorticon
{

CSaveMenu::CSaveMenu() :
VorticonMenu(GsRect<float>(0.1f, 0.0f, 0.8f, 1.0f) ),
m_overwrite(false)
{
	// Load the state-file list
    std::vector<std::string> StateFileList;
    gpSaveGameController->readSlotList(StateFileList);

	for(Uint32 i=1;i<=8;i++)
	{
		std::string text = EMPTY_TEXT;
		if(i <= StateFileList.size())
			text = StateFileList.at(i-1);

        mpMenuDialog->addControl(new InputText( text ) );
	}
}


void CSaveMenu::refresh()
{
    // Load the state-file list
    std::vector<std::string> StateFileList;
    gpSaveGameController->readSlotList(StateFileList);

    std::list< std::shared_ptr<CGUIControl> > &list = mpMenuDialog->getControlList();

    auto itCtrl = list.begin();
    itCtrl++;

    for(Uint32 i=0 ;i<8 ; i++)
    {
        std::string text = EMPTY_TEXT;
        if(i < StateFileList.size())
            text = StateFileList.at(i);

        std::shared_ptr<CGUIControl> &ctrl = *itCtrl;
        InputText *input = dynamic_cast<InputText*>( ctrl.get() );

        input->setText(text);
    }
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

#ifdef NOTYPESAVE
                const std::string saveText = getTimeStr();

#else
				if(pInput->Typing())
                {
                    const std::string saveText = pInput->getText();
#endif
                    gpSaveGameController->prepareSaveGame( sel, saveText );
                    g_pBehaviorEngine->setPause(false);
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
    InputText *pInput = dynamic_cast<InputText*>(mpMenuDialog->CurrentControl());

    if(pInput)
    {
        pInput->setTypeMode(false);
    }
}

}
