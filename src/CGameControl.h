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
#include "common/options.h"
#include "vorticon/CEGAGraphics.h"
#include "vorticon/CMessages.h"
#include "vorticon/CPassive.h"
#include "vorticon/playgame/CPlayGame.h"
#include <string>

#define LOADGFX 0x01
#define LOADSTR 0x02
#define LOADSND 0x04
#define LOADALL 0xFF

class CGameControl {
public:
	
	enum GameMode{
		GAMELAUNCHER, PASSIVE, PLAYGAME, SHUTDOWN
	};
	
	CGameControl();
	virtual ~CGameControl();
	
	bool init(int argc, char *argv[]);
	bool init(char mode=GAMELAUNCHER);
	bool loadResources(unsigned short Episode, const std::string& DataDirectory, Uint8 flags=LOADALL);
	
	void process();
	
	
	void cleanup(char mode);
	void cleanup() { cleanup(m_mode); }
	
	
	// getters and setters
	bool mustShutdown(){ return (m_mode==SHUTDOWN); }
	
	char m_mode;
	stOption *mp_option;
	
private:
	CGameLauncher *mp_GameLauncher;
	CPassive *mp_PassiveMode;
	CPlayGame *mp_PlayGame;
	
	bool m_endgame;
	bool m_show_finale;
	Uint8 m_Episode;
	Uint8 m_Numplayers;
	Uint8 m_ChosenGame;
	Uint8 m_Difficulty;
	std::string m_DataDirectory;
	int current_demo;
	Uint8 m_startLevel;
	
	CEGAGraphics *m_EGAGraphics;
	CMessages *m_Messages;
};

#endif /* CGAMECONTROL_H_ */
