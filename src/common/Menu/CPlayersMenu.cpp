/*
 * CPlayersMenu.cpp
 *
 *  Created on: 19.05.2010
 *      Author: gerstrong
 */

#include "CPlayersMenu.h"

#include "../../StringUtils.h"
#include "../../keen.h"

#define SAFE_DELETE(x) if(x) { delete x; x = NULL; }

CPlayersMenu::CPlayersMenu(Uint8 &players, Uint8 dlg_theme) :
CBaseMenu(dlg_theme),
m_players(players)
{
	mp_Dialog = new CDialog(13, MAX_PLAYERS+2,INPUT_MODE_UP_DOWN, dlg_theme);
	int i;

	mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, 1, "1 Player");
	for(i=2;i<=MAX_PLAYERS;i++)
	{
		mp_Dialog->addObject(DLG_OBJ_OPTION_TEXT, 1, i, itoa(i)+" Player");
	}
}

void CPlayersMenu::processSpecific()
{
	if( m_selection != NO_SELECTION)
	{
		if( m_selection < MAX_PLAYERS )
		{
			m_players = m_selection + 1;
			m_mustclose = true;
		}
	}
}


CPlayersMenu::~CPlayersMenu()
{
	SAFE_DELETE(mp_Dialog);
}
