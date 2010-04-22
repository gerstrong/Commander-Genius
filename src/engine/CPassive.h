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
#include "../common/CMenuVorticon.h"
#include "../common/CMap.h"
#include "../common/CObject.h"
#include <string>
#include <vector>


class CPassive {
public:
	enum Modes{
		INTRO, TITLE, DEMO, STARTGAME, SHUTDOWN
	};
	
	CPassive(char Episode, std::string DataDirectory,
			 CSavedGame &SavedGame, stOption *p_Option);
	virtual ~CPassive();
	
	virtual bool init(char mode = INTRO) {return false;};
	
	virtual void process() {};
	
	// Getters
	char getEpisode() { return m_Episode; }
	char getNumPlayers() { return m_NumPlayers; }
	char getDifficulty() { return m_Difficulty; }
	std::string getGamePath() { return m_DataDirectory; }
	
	bool getchooseGame() { return m_modeg; }
	bool mustStartGame() { return (m_mode==STARTGAME); }
	bool getExitEvent() { return (m_mode==SHUTDOWN); }
	
	virtual void cleanup() {};
	
	char m_mode;
	
	CMenuVorticon *mp_Menu;

protected:
	char m_Episode;
	char m_NumPlayers;
	char m_Difficulty;
	bool m_modeg;
	std::string m_DataDirectory;

	CSavedGame &m_SavedGame;
};
#endif /* CPASSIVE_H_ */
