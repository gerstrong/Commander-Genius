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
#include "CObject.h"
#include "Menu/CBaseMenu.h"

#include "../sdl/CInput.h"
#include "../fileio/CSavedGame.h"
#include "../fileio/CExeFile.h"

class CMenu : public CBaseMenu
{
public:
	CMenu(char menu_mode, CExeFile &ExeFile,
			 CSavedGame &SavedGame,
			 stOption *pOption, Uint8 DlgTheme,
			 bool &restartVideo );

	bool getChooseGame() { return m_choosegame; }
	bool getExitEvent() { return m_quit; }

	// initialization functions
	void init( menutypes menu_type );
	void initMainMenu();
	void initF1Menu();

	void process();

	// Process functions
	void processMainMenu();

	void cleanup();

	virtual ~CMenu();

	bool m_demoback;

	CDialog *mp_Dialog;

protected:
	char m_Episode;
	CExeFile &m_ExeFile;
	std::string m_GamePath;
	CSavedGame &m_SavedGame;
	stOption *mp_option;
	char m_menu_mode;

	bool m_choosegame;
	bool m_overwrite;
	bool m_Endgame;
	bool m_quit;
	int m_selection;
	char m_lastselect;
	menutypes m_menu_type;
	Uint8 m_NumPlayers;
	Sint8 m_Difficulty;
	int  m_saveslot;
	bool &m_restartVideo;

	CBaseMenu *mp_SubMenu;

private:
	std::vector<std::string> m_StateFileList;
};


#endif /* CMENU_H_ */
