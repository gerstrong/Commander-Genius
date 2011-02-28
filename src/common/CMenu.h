/*
 * CMenu.h
 *
 *  Created on: 22.04.2010
 *      Author: gerstrong
 */

#ifndef CMENU_H_
#define CMENU_H_

#include <string>
#include <list>
#include "options.h"
#include "CObject.h"
#include "Menu/CBaseMenu.h"
#include "sdl/CInput.h"
#include "fileio/CSavedGame.h"
#include "fileio/CExeFile.h"
#include "common/CBehaviorEngine.h"

class CMenu : public CBaseMenu
{
public:
	CMenu(char menu_mode, CExeFile &ExeFile,
			 CSavedGame &SavedGame,
			 Uint8 DlgTheme,
			 bool &restartVideo );

	// Getters
	bool getChooseGame() { return m_choosegame; }
	bool getExitEvent() { return m_quit; }
	char getNumPlayers() { return m_NumPlayers; }
	char getDifficulty() { return m_Difficulty; }
	bool mustEndGame() { return m_Endgame; }
	bool mustStartGame() { return (m_NumPlayers > 0 and m_Difficulty > 0); }
	bool getShowStory() { return (m_menu_type==STORY); }
	bool getBacktoDemo() { return m_demoback; }


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
	char m_menu_mode;

	bool m_choosegame;
	bool m_overwrite;
	bool m_Endgame;
	bool m_quit;
	int m_selection;
	char m_lastselect;
	menutypes m_menu_type;
	Uint8 m_NumPlayers;
	Uint8 m_Difficulty;
	int  m_saveslot;
	bool &m_restartVideo;

	std::list<CBaseMenu*> m_SubMenus;

private:
	std::vector<std::string> m_StateFileList;
};


#endif /* CMENU_H_ */
