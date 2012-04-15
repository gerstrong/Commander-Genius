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
#include "engine/infoscenes/CInfoScene.h"
#include "fileio/CSaveGameController.h"
#include <string>

///////////////////////
// Events Structure  //
///////////////////////
// This event switches to the PassiveMode
struct GMSwitchToPassiveMode : CEvent {
	const std::string m_DataDirectory;
	const int m_Episode;
	GMSwitchToPassiveMode( const std::string& DataDirectory, const int& Episode ) :
		m_DataDirectory(DataDirectory),
		m_Episode(Episode){}
};

// This event switches to the PlayGameMode
struct GMSwitchToPlayGameMode : CEvent
{
	const int m_Episode;
	const int m_Numplayers;
	const int m_Difficulty;
	const std::string m_DataDirectory;
	CSaveGameController m_SavedGame;
	const int m_startlevel;

	GMSwitchToPlayGameMode( const int Episode, const int Numplayers,
			const int Difficulty, const std::string& DataDirectory, CSaveGameController& SavedGame, const int startlevel = -1 ) :
				m_Episode(Episode),
				m_Numplayers(Numplayers),
				m_Difficulty(Difficulty),
				m_DataDirectory(DataDirectory),
				m_SavedGame(SavedGame),
				m_startlevel(startlevel){}
};

// It's a simple quit event which will force CG to close the App
struct GMQuit : CEvent {};

// It's a simple quit event which will force CG to close the App
struct GMStart : CEvent
{
	GMStart(int &Slot) :
	mSlot(Slot)
	{}

	int &mSlot;
};

// Start the Gameplay
struct StartGameplayEvent : CEvent {};


///////
// Base Class for the modes
//////
class CGameMode
{
public:
	CGameMode() {}

	virtual void process() = 0;
	virtual void init() = 0;
};

#endif /* CGAMEMODE_H_ */
