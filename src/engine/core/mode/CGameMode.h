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

#include <base/GsEvent.h>

//#include "core/LogicDrawSplitter.h"
#include <string>

///////////////////////
// Events Structure  //
///////////////////////
// This event switches to the PassiveMode
struct GMSwitchToPassiveMode : CEvent
{};

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

// Event for opening the download Dialog
struct GMDownloadDlgOpen : CEvent {};

// It's a simple start event which will force CG launch a game
struct GMStart : CEvent {};

struct GMPatchSelected : CEvent {};

struct GMDosExecSelected : CEvent {};


// Happens when in the menu the user decides to end the game or he reached
// the finale
struct EventEndGamePlay : CEvent {};

///////
// Base Class for the modes
//////
class CGameMode
{
public:
	CGameMode() {}
	virtual ~CGameMode() {}

    virtual void pumpEvent(const CEvent *evPtr) = 0;

    virtual bool init() = 0;
    virtual void ponder(const float deltaT) = 0;
    virtual void render() = 0;
};


#endif /* CGAMEMODE_H_ */
