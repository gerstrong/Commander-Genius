/*
 * CPassive.h
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong and pizza2004
 *
 *
 * TODO: Comment about this class
 */
#ifndef CMENU_H_
#define CMENU_H_

#include <string>
#include <SDL/SDL.h>
#include "../dialog/CDialog.h"
#include "../dialog/CTextViewer.h"

#include "infoscenes/CInfoScene.h"

#define TWIRL_SPEED_SLOW        100
#define TWIRL_SPEED_FAST        20

#define MENUS_Y                 32
#define MAINMENU_X              32

#define STORYBOARD_X			672
#define STORYBOARD_Y			32

#define FONT_TWIRL              9
#define TWIRL_SPEED             30
#define TWIRL_NUM_FRAMES        5

class CMenu {
	
public:
	// Which Menu has to be shown?
	
	enum menutypes{
		MAIN, NEW,
		CONTROLPLAYERS, STORY,
		HIGHSCORES, ABOUTCG,
		ABOUTID, ORDERING,
		LOAD, SAVE, START,
		DIFFICULTY, CONFIGURE,
		GRAPHICS, CONTROLS,
		AUDIO, OPTIONS,
		F1, HELP, ENDGAME, QUIT
	};
	
	// Active means, when the player is playing, PASSIVE when the Player is not playing
	enum menumodes{
		ACTIVE, PASSIVE
	};
	
	CMenu( char menu_mode, std::string &GamePath, char &Episode );
	virtual ~CMenu();
	
	bool init( char menu_type = MAIN );
	
	void process();
	void processMainMenu();
	void processQuitMenu();
	void processNumPlayersMenu();
	void processDifficultyMenu();
	void processConfigureMenu();
	void processNumControlMenu();
	void processF1Menu();
	void processEndGameMenu();
	void processSaveMenu();
	
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
	
	bool m_demoback;
	char m_mode;
	
private:
	void initMainMenu();
	void initConfirmMenu();
	void initNumPlayersMenu();
	void initDifficultyMenu();
	void initConfigureMenu();
	void initSaveMenu();
	void initNumControlMenu();
	void initF1Menu();
	
	CDialog *mp_Dialog;
	CTextViewer *mp_TextViewer;
	CInfoScene *mp_InfoScene;
	SDL_Surface *mp_MenuSurface;
	char &m_Episode;
	std::string &m_GamePath;
	
	bool m_choosegame;
	bool m_goback;
	bool m_Endgame;
	bool m_quit;
	char m_selection;
	char m_menu_type;
	char m_menu_mode;
	char m_NumPlayers;
	char m_Difficulty;
};

#endif /* CMENU_H_ */
