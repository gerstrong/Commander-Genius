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
#include "fileio/CSavedGame.h"
#include <string>

///////////////////////
// Events Structure  //
///////////////////////
// This event switches to the GameLauncher
struct GMSwitchToGameLauncher : CEvent {
	const int m_ChosenGame;
	const int m_StartLevel;
	GMSwitchToGameLauncher(	const int ChosenGame=-1, const int StartLevel=-1 ) :
		m_ChosenGame(ChosenGame),
		m_StartLevel(StartLevel){}
};

// This event switches to the PassiveMode
struct GMSwitchToPassiveMode : CEvent {
	const std::string m_DataDirectory;
	const int m_Episode;
	GMSwitchToPassiveMode( const std::string& DataDirectory, const int& Episode ) :
		m_DataDirectory(DataDirectory),
		m_Episode(Episode){}
};

// This event switches to the PlayGameMode
struct GMSwitchToPlayGameMode : CEvent {
	const int m_Episode;
	const int m_Numplayers;
	const int m_Difficulty;
	const std::string m_DataDirectory;
	CSavedGame m_SavedGame;
	const int m_startlevel;

	GMSwitchToPlayGameMode( const int Episode, const int Numplayers,
			const int Difficulty, const std::string& DataDirectory, CSavedGame& SavedGame, const int startlevel = -1 ) :
				m_Episode(Episode),
				m_Numplayers(Numplayers),
				m_Difficulty(Difficulty),
				m_DataDirectory(DataDirectory),
				m_SavedGame(SavedGame),
				m_startlevel(startlevel){}
};

// It's a simple quit event which will force CG to close the App
struct GMQuit : CEvent {};

///////
// Base Class for the modes
//////
class CGameMode {
public:
	CGameMode();

	virtual void process() = 0;
	virtual void init() = 0;
};

#endif /* CGAMEMODE_H_ */
