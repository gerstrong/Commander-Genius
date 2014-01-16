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
#include "common/CSpriteObject.h"
#include "core/mode/CGameMode.h"
#include "fileio/CSaveGameController.h"
#include <string>
#include <vector>
#include <memory>


class CPassive : public CGameMode
{
public:

	enum Modes
	{
		INTRO, TITLE, DEMO, STARTGAME, SHUTDOWN
	};
	
	CPassive();
	
    void pumpEvent(const CEvent *evPtr);

    //virtual bool init(char mode) {return false;}

    virtual bool init() = 0;
	
    virtual void ponder(const float deltaT) = 0;

    virtual void render() = 0;
	
	// Getters
	char getEpisode() { return m_Episode; }
	char getDifficulty() { return m_Difficulty; }
	CSaveGameController getSavedGameBlock() { return m_SavedGame; }
	std::string getGamePath() { return m_DataDirectory; }
	
	bool getchooseGame() { return m_modeg; }
	bool mustStartGame() { return (m_mode==STARTGAME); }
	bool getExitEvent() { return (m_mode==SHUTDOWN); }
	
    virtual void cleanup() {}
	
	void createOutlinedText(const int splash_x, const int splash_y, const std::string &text);

	char m_mode;
	
protected:
	char m_Episode;
	char m_Difficulty;
	
	// TODO: Bad variable name. Think about something better
	bool m_modeg;
	std::string m_DataDirectory;
	CSaveGameController m_SavedGame;
	std::shared_ptr<SDL_Surface> mpTextSfc;
};
#endif /* CPASSIVE_H_ */
