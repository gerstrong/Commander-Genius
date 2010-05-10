/*
 * CMenu.h
 *
 *  Created on: 22.04.2010
 *      Author: gerstrong
 */

#ifndef CMENU_H_
#define CMENU_H_

#include <string>
#include "options.h"
#include "Menu/CBaseMenu.h"
#include "../fileio/CSavedGame.h"

class CMenu
{
public:
	CMenu(char menu_mode, std::string &GamePath,
			 char &Episode, CSavedGame &SavedGame,
			 stOption *pOption );

	bool m_demoback;
	bool m_hideobjects;

	CDialog *mp_Dialog;

	virtual ~CMenu();

protected:
	char &m_Episode;
	std::string &m_GamePath;
	CSavedGame &m_SavedGame;
	bool m_RestartVideo;
	stOption *mp_option;
	char m_menu_mode;

	bool m_choosegame;
	bool m_overwrite;
	bool m_goback;
	bool m_goback2;
	bool m_Endgame;
	bool m_quit;
	int m_selection;
	char m_lastselect;
	char m_menu_type;
	char m_NumPlayers;
	char m_Difficulty;
	int  m_saveslot;

	CBaseMenu *mp_Menu;
	std::map<int,int> m_menuback;
	std::map<int,int> m_menumap;
};


#endif /* CMENU_H_ */
