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

#include <ctime>

CSaveMenu::CSaveMenu(Uint8 dlg_theme) :
CBaseMenu(dlg_theme, CRect<float>(0.0f, 0.1f, 1.0f, 0.8f) ),
mp_OverwriteMenu(NULL),
m_overwrite(false)
{

	mpMenuDialog->setBackground( CGUIDialog::VORTICON );

	std::string text;

	// Load the state-file list
	std::vector<std::string> StateFileList = gpSaveGameController->getSlotList();

	for(Uint32 i=1;i<=20;i++)
	{
		text = ""jklhn;
		if(i <= StateFileList.size())
			text = StateFileList.at(i-1);

		mpMenuDialog->addControl(new CGUIInputText( text, CGUIInputText::VORTICON ) );
	}
}

/*void CSaveMenu::processSpecific()
{*/
	/*if(!mp_OverwriteMenu)
	{
		if( m_selection != NO_SELECTION )
		{
			if( mp_Dialog->getInputMode(INPUT_MODE_UP_DOWN) )
			{
				if( m_SavedGame.getSlotList().size() > m_selection )
					mp_Dialog->m_name = m_SavedGame.getSlotList().at(m_selection);

				if( m_SavedGame.getSlotList().size() > m_selection && mp_Dialog->m_name != m_SavedGame.getEmptyString() )
				{
					mp_OverwriteMenu = new CConfirmMenu("Overwrite?", m_overwrite, m_dlg_theme);
					m_suspended = true;
				}
				else
				{
#ifndef NOKEYBOARD
					mp_Dialog->m_name = "Slot" + itoa(m_selection);
					mp_Dialog->m_length = TEXT_WIDTH;
					mp_Dialog->setInputMode(INPUT_MODE_TEXT);
#else

					mp_Dialog->m_name = m_SavedGame.getUnnamedSlotName();
					mp_Dialog->m_length = TEXT_WIDTH;

					// save right away
					saveSelection();
#endif
					return;
				}

			}
			else if ( mp_Dialog->getInputMode(INPUT_MODE_TEXT) &&
					(g_pInput->getPressedKey(KENTER) || g_pInput->getPressedKey(KCTRL) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_STATUS) ) )
			{
				saveSelection();
			}
		}
	}
	else
	{
		mp_OverwriteMenu->processCommon();
		mp_OverwriteMenu->processSpecific();
		mp_OverwriteMenu->postProcess();

		if(mp_OverwriteMenu->mustClose())
		{
			SAFE_DELETE(mp_OverwriteMenu);
			m_suspended = false;
			mp_Dialog->m_length = TEXT_WIDTH;

			if(m_overwrite)
			{
#ifndef NOKEYBOARD
				mp_Dialog->setInputMode(INPUT_MODE_TEXT);
#else
				mp_Dialog->m_name = m_SavedGame.getUnnamedSlotName();
				mp_Dialog->m_length = TEXT_WIDTH;
				saveSelection();
#endif

			}
			else
			{
				mp_Dialog->setInputMode(INPUT_MODE_UP_DOWN);
				m_selection = NO_SELECTION;
			}

		}
	}*/
/*}

void CSaveMenu::saveSelection()
{*/
	/*mp_Dialog->setObjectText(m_selection, mp_Dialog->m_name);

	m_SavedGame.prepareSaveGame(m_selection+1, mp_Dialog->m_name);
	m_selection = NO_SELECTION;
	mp_Dialog->setInputMode(INPUT_MODE_UP_DOWN);*/
//}

