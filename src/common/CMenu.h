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
#include "../fileio/CSavedGame.h"

class CMenu
{
public:
	CMenu(char menu_mode, std::string &GamePath,
			 char &Episode, CSavedGame &SavedGame,
			 stOption *pOption );
	virtual ~CMenu();

protected:
	char &m_Episode;
	std::string &m_GamePath;
	CSavedGame &m_SavedGame;
	bool m_RestartVideo;
	stOption *mp_option;
};


#endif /* CMENU_H_ */
