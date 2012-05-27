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

#include "CTitle.h"
#include "common/CMap.h"
#include "common/CObject.h"
#include "gui/CGUIDialog.h"
#include "fileio/CSaveGameController.h"
#include <string>
#include <vector>


class CPassive
{
public:

	enum Modes
	{
		INTRO, TITLE, DEMO, STARTGAME, SHUTDOWN
	};
	
	CPassive();
	
	virtual bool init(char mode = INTRO) {return false;};
	
	virtual void process() {};
	
	// Getters
	char getEpisode() { return m_Episode; }
	char getDifficulty() { return m_Difficulty; }
	CSaveGameController getSavedGameBlock() { return m_SavedGame; }
	std::string getGamePath() { return m_DataDirectory; }
	
	bool getchooseGame() { return m_modeg; }
	bool mustStartGame() { return (m_mode==STARTGAME); }
	bool getExitEvent() { return (m_mode==SHUTDOWN); }
	
	virtual void cleanup() {};
	
	char m_mode;
	
protected:
	char m_Episode;
	char m_Difficulty;
	bool m_modeg;
	std::string m_DataDirectory;
	CSaveGameController m_SavedGame;
};
#endif /* CPASSIVE_H_ */
