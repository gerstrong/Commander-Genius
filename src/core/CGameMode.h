/*
 * CGameMode.h
 *
 *  Created on: 26.03.2011
 *      Author: gerstrong
 *
 *  Base class responsible for controlling the CG Modes which are Launcher Menu, Passive and Play Mode
 */

#ifndef CGAMEMODE_H_
#define CGAMEMODE_H_

#include "engine/CEvent.h"

#include "common/CGameLauncher.h"
#include "engine/CPassive.h"
#include "engine/playgame/CPlayGame.h"
#include "engine/CEGAGraphics.h"

// CG Core Modi
enum GameMode{
	GM_GAMELAUNCHER, GM_PASSIVE, GM_PLAYGAME, GM_QUIT
};

// There is only one event that is triggered called ChangeMode.
// It is passed with the Mode which tells to what mode to change...
struct ChangeMode : CEvent {
	const GameMode Mode;
	ChangeMode(const GameMode &l_Mode) : Mode(l_Mode){}
};

class CGameMode {
public:
	CGameMode(Uint8& Episode, Uint8& Numplayers,
			Uint8& Difficulty, std::string& DataDirectory);
	virtual ~CGameMode();

	virtual void process() = 0;
	virtual bool init() = 0;

	CGameLauncher *mp_GameLauncher;
	CPassive *mp_PassiveMode;
	CPlayGame *mp_PlayGame;

	CSavedGame m_SavedGame;

	bool m_endgame;

	Uint8& m_Episode;
	Uint8& m_Numplayers;
	Uint8& m_Difficulty;
	std::string& m_DataDirectory;
	int current_demo;
};

#endif /* CGAMEMODE_H_ */
