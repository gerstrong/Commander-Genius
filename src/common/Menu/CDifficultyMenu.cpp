/*
 * CDifficultyMenu.cpp
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#include "CDifficultyMenu.h"

#define SAFE_DELETE(x)	if(x) { delete x; x = NULL; }

CDifficultyMenu::CDifficultyMenu( Sint8 &difficulty, Uint8 &players, Uint8 dlg_theme ) :
CBaseMenu(dlg_theme),
m_difficulty(difficulty),
m_players(players)
{
	mp_Dialog = new CDialog(11, 5, INPUT_MODE_UP_DOWN,m_dlg_theme);
	mp_PlayersMenu = new CPlayersMenu( m_players, m_dlg_theme );
	m_suspended = true;

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "Easy");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 2, "Normal");
	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 3, "Hard");

	mp_Dialog->processInput(1);
}

void CDifficultyMenu::processSpecific()
{
	if(mp_PlayersMenu)
	{
		mp_PlayersMenu->processCommon();
		mp_PlayersMenu->processSpecific();
		mp_PlayersMenu->postProcess();

		if(mp_PlayersMenu->mustClose())
		{
			SAFE_DELETE(mp_PlayersMenu);
			m_suspended = false;
		}
	}
	else
	{
		if( m_selection != NO_SELECTION)
			m_difficulty = m_selection;
	}
}

CDifficultyMenu::~CDifficultyMenu()
{
	// TODO Auto-generated destructor stub
}
