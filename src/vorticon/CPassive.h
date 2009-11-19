/*
 * CPassive.h
 *
 *  Created on: 24.09.2009
 *      Author: gerstrong
 *
 *
 *  This Class controls the 3 screens that are shown
 *  when the game is started. THese are:
 *  Intro, Title and Demos.
 *  In that mode you can press any key to open the menu
 */

#ifndef CPASSIVE_H_
#define CPASSIVE_H_

#include "CIntro.h"
#include "CTitle.h"
#include "../dialog/CTextBox.h"
#include "../common/CMenu.h"
#include "../common/CMap.h"
#include "../common/CObject.h"
#include <string>
#include <vector>


class CPassive {
public:
	enum Modes{
		INTRO, TITLE, DEMO, STARTGAME, SHUTDOWN
	};
	
	CPassive(char Episode, std::string DataDirectory);
	virtual ~CPassive();
	
	bool init(char mode = INTRO);
	
	void process();
	
	// Getters
	char getEpisode() { return m_Episode; }
	char getNumPlayers() { return m_NumPlayers; }
	char getDifficulty() { return m_Difficulty; }
	std::string getGamePath() { return m_DataDirectory; }
	
	bool getchooseGame() { return m_modeg; }
	bool mustStartGame() { return (m_mode==STARTGAME); }
	bool getExitEvent() { return (m_mode==SHUTDOWN); }
	
	void cleanup();
	
	char m_mode;
	
	CMenu *mp_Menu;
	
private:
	CIntro *mp_IntroScreen;
	CTitle *mp_TitleScreen;
	CMap *mp_Map;
	CTextBox *mp_PressAnyBox;
	
	CTilemap *mp_Tilemap;
	SDL_Surface *mp_Scrollsurface;
	
	std::vector<CObject*> m_object;
	
	int m_textsize;
	bool m_GoDemo;
	bool m_modeg;
	bool m_hideobjects;
	char *m_text;
	char m_Episode;
	char m_NumPlayers;
	char m_Difficulty;
	std::string m_DataDirectory;
};
#endif /* CPASSIVE_H_ */
