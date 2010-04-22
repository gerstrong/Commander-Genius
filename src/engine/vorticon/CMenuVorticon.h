/*
 * CMenuVorticon.h
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong and pizza2004
 *
 *	This is the superclass which will be used for getting
 *	a menu work and is extended with specific engine instructions
 * */
#ifndef CMENUVORTICON_H_
#define CMENUVORTICON_H_

#include <SDL.h>
#include "../../dialog/CDialog.h"
#include "../../dialog/CTextViewer.h"
#include "../../common/Menu/CBaseMenu.h"
#include "../../common/CMap.h"

#include "../../engine/infoscenes/CInfoScene.h"

#include "../../common/CMenu.h"

#define TWIRL_SPEED_SLOW        100
#define TWIRL_SPEED_FAST        20

#define MENUS_Y                 32
#define MAINMENU_X              32

#define STORYBOARD_X			672
#define STORYBOARD_Y			32

#define FONT_TWIRL              9
#define TWIRL_SPEED             30
#define TWIRL_NUM_FRAMES        5

class CMenuVorticon : public CMenu {

public:

	CMenuVorticon( char menu_mode, std::string &GamePath,
			char &Episode, CMap &Map,
			CSavedGame &SavedGame, stOption *pOption );

	bool init( char menu_type = MAIN );

	void process();
	void processMainMenu();
	void processQuitMenu();
	void processNumPlayersMenu();
	void processDifficultyMenu();
	void processNumControlMenu();
	void processF1Menu();
	void processEndGameMenu();
	void processSaveMenu();
	void processLoadMenu();
	void processOverwriteMenu();
	void processDebugMenu();
	void processModMenu();

	void (CMenuVorticon::*processPtr)();
	
	void cleanup();

	// Getters
	char getNumPlayers() { return m_NumPlayers; }
	char getDifficulty() { return m_Difficulty; }
	bool mustEndGame() { return m_Endgame; }
	bool mustStartGame() { return (m_NumPlayers > 0 and m_Difficulty > -1); }
	bool getExitEvent() { return (m_quit); }
	bool getShowStory() { return (m_menu_type==STORY); }
	bool mustBeClosed() { return m_goback; }
	bool getBacktoDemo() { return m_demoback; }
	bool getChooseGame() { return m_choosegame; }
	bool restartVideo() { return m_RestartVideo; }
	
	void videoRestarted() { m_RestartVideo = false; }

	bool m_demoback;
	bool m_hideobjects;
	char m_mode;
	
	CDialog *mp_Dialog;

	virtual ~CMenuVorticon();

private:
	void initMainMenu();
	void initConfirmMenu(std::string confirmtext = "  Are you certain?  ");
	void initNumPlayersMenu();
	void initDifficultyMenu();
	void initConfigureMenu();
	void initSaveMenu();
	void initNumControlMenu();
	void initControlMenu();
	void initF1Menu();
	void initOptionsMenu();
	void initAudioMenu();
	void initDebugMenu();
	void initModMenu();

	CInfoScene *mp_InfoScene;
	CBaseMenu *mp_Menu;
	std::map<int,int> m_menuback;
	CMap &m_Map;
	std::map<int,int> m_menumap;
	std::vector<std::string> m_StateFileList;

	bool m_choosegame;
	bool m_overwrite;
	bool m_goback;
	bool m_goback2;
	bool m_Endgame;
	bool m_quit;
	int m_selection;
	char m_lastselect;
	char m_menu_type;
	char m_menu_mode;
	char m_NumPlayers;
	char m_Difficulty;
	int  m_saveslot;
};

#endif /* CMENUVORTICON_H_ */
