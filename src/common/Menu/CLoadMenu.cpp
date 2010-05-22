/*
 * CLoadMenu.cpp
 *
 *  Created on: 22.05.2010
 *      Author: gerstrong
 */

#include "CLoadMenu.h"
#include "../../fileio/CSavedGame.h"

CLoadMenu::CLoadMenu(Uint8 dlg_theme, CSavedGame &SavedGame) :
CBaseMenu(dlg_theme),
m_SavedGame(SavedGame)
{
	std::string text;
	mp_Dialog = new CDialog(22, 22, INPUT_MODE_UP_DOWN, m_dlg_theme);

	// Load the state-file list
	std::vector<std::string> StateFileList = m_SavedGame.getSlotList();

	for(Uint32 i=1;i<=20;i++)
	{
		text = "";
		if(i <= StateFileList.size())
		{
			text = StateFileList.at(i-1);
			mp_Dialog->m_name = text;
		}
		if(text == "")
			text = "     EMPTY       ";
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, text);
	}
}

void CLoadMenu::processSpecific()
{
	if( m_selection != NO_SELECTION)
	{
		if(mp_Dialog->m_name == "     EMPTY       ")
		{
			//TODO: Message saying can't load, it is empty.  Also, we need to add a check to
			// see if it is corrupt, or something to prevent old saves from crashing due to incompatibility.
		}
		else
		{
			m_SavedGame.prepareLoadGame(m_selection + 1);
		}
	}
}

CLoadMenu::~CLoadMenu()
{
	// TODO Auto-generated destructor stub
}
