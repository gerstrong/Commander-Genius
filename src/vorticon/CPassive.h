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
#include "../common/CMenu.h"
#include "../common/CMap.h"
#include <string>


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

private:
	CIntro *mp_IntroScreen;
	CTitle *mp_TitleScreen;
	CMenu *mp_Menu;
	CMap *mp_Map;

	CTilemap *mp_Tilemap;
	SDL_Surface *mp_Scrollsurface;

	int m_textsize;
	bool m_GoDemo;
	bool m_modeg;
	char *m_text;
	char m_Episode;
	char m_NumPlayers;
	char m_Difficulty;
	std::string m_DataDirectory;

	char m_mode;
};
#endif /* CPASSIVE_H_ */
