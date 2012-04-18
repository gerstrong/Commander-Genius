/*
 * CSaveMenu.cpp
 *
 *  Created on: 21.05.2010
 *      Author: gerstrong
 */

#include "CSaveMenu.h"
#include "CBaseMenu.h"
#include "CConfirmMenu.h"
#include "sdl/input/CInput.h"
#include "gui/CGUIInputText.h"
#include "fileio/CSaveGameController.h"
#include "common/CBehaviorEngine.h"
#include "common/Menu/CMenuController.h"

CSaveMenu::CSaveMenu() :
CBaseMenu(CRect<float>(0.1f, 0.0f, 0.8f, 1.0f) ),
mp_OverwriteMenu(NULL),
m_overwrite(false)
{

	std::string text;

	// Load the state-file list
	std::vector<std::string> StateFileList = gpSaveGameController->getSlotList();

	for(Uint32 i=1;i<=20;i++)
	{
		text = "";
		if(i <= StateFileList.size())
			text = StateFileList.at(i-1);

		mpMenuDialog->addControl(new CGUIInputText( text, CGUIInputText::VORTICON ) );
	}
}

void CSaveMenu::sendEvent(SmartPointer<CEvent> command)
{
	// Before all events are sent to the dialog which handles selection catch some specific events
	// required for the saving process.
	if( CommandEvent *ev = dynamic_cast<CommandEvent*>(command.get()) )
	{
		const int sel = mpMenuDialog->Selection();
		if( sel > 0 )
		{
			if(ev->mCommand == IC_JUMP || ev->mCommand == IC_STATUS)
			{
				const CGUIInputText *pInput = dynamic_cast<CGUIInputText*>(mpMenuDialog->CurrentControl());
				gpSaveGameController->prepareSaveGame( sel, pInput->getText() );
				g_pBehaviorEngine->EventList().add( new CloseAllMenusEvent() );
				return;
			}
		}
	}

	mpMenuDialog->sendEvent(command);
}

