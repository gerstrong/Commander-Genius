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
struct GMSwitchToPassiveMode : CEvent
{
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
	const std::string m_DataDirectory;
	const int m_startlevel;

	GMSwitchToPlayGameMode( const int Episode, const int Numplayers,
								const std::string& DataDirectory, const int startlevel = -1 ) :
				m_Episode(Episode),
				m_Numplayers(Numplayers),
				m_DataDirectory(DataDirectory),
				m_startlevel(startlevel)
				{}
};

// It's a simple quit event which will force CG to close the App
struct GMQuit : CEvent {};

// It's a simple start event which will force CG launch a game from a specified slot
struct GMStart : CEvent
{
	GMStart(int &Slot) :
	mSlot(Slot)
	{}

	int &mSlot;
};

///////
// Base Class for the modes
//////
class CGameMode
{
public:
	CGameMode() {}
	virtual ~CGameMode() {}

	virtual void process() = 0;
	virtual void init() = 0;
};


#endif /* CGAMEMODE_H_ */
