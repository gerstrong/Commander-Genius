/*
 * CGameControl.h
 *
 *  Created on: 22.09.2009
 *      Author: gerstrong
 *
 *  Auxiliary Class for CGame. It only tell the Game-Engine what to do.
 */

#ifndef CGAMECONTROL_H_
#define CGAMECONTROL_H_

#include "common/CGameLauncher.h"
#include "common/CTeleporter.h"
#include "common/options.h"
#include "fileio/CSavedGame.h"
#include "vorticon/CEGAGraphics.h"
#include "vorticon/CMessages.h"
#include "vorticon/CPassive.h"
#include "vorticon/playgame/CPlayGame.h"

#include "galaxy/CEGAGraphicsGalaxy.h"

#include <string>

enum load_states{
LOADGFX=0x01,
LOADSTR=0x02,
LOADSND=0x04,
LOADALL=0xFF
};

class CGameControl {
public:
	
	enum GameMode{
		GAMELAUNCHER, PASSIVE, PLAYGAME, SHUTDOWN
	};
	
	CGameControl();
	
	bool init(int argc, char *argv[]);
	bool init(char mode=GAMELAUNCHER);
	bool loadResources(unsigned short Episode, const std::string& DataDirectory, Uint8 flags=LOADALL);
	
	void process();
	
	void cleanup(char mode);
	void cleanupAll();
	
	// getters and setters
	bool mustShutdown(){ return (m_mode==SHUTDOWN); }
	
	char m_mode;
	stOption *mp_option;
	
	virtual ~CGameControl();

private:
	CGameLauncher *mp_GameLauncher;
	CPassive *mp_PassiveMode;
	CPlayGame *mp_PlayGame;
	CSavedGame m_SavedGame;
	
	bool m_endgame;
	bool m_show_finale;
	Uint8 m_Episode;
	Uint8 m_Numplayers;
	Uint8 m_ChosenGame;
	Uint8 m_Difficulty;
	std::string m_DataDirectory;
	int current_demo;
	Uint8 m_startLevel;
	
	std::vector<stTeleporterTable> m_TeleporterTable; // Teleporter table used for the destinations

	CEGAGraphics *m_EGAGraphics;
};

#endif /* CGAMECONTROL_H_ */
