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
#include "vorticon/CEGAGraphics.h"
#include "vorticon/CMessages.h"
#include "vorticon/CPassive.h"
#include "fileio/CTileLoader.h"
#include <string>

class CGameControl {
public:

	enum GameMode{
		GAMELAUNCHER, PASSIVE, PLAYGAME, SHUTDOWN
	};

	CGameControl();
	virtual ~CGameControl();

	bool init();
	bool loadResources(unsigned short Episode, const std::string& DataDirectory);

	void process();

	void cleanup();

	// getters and setters
	bool mustShutdown(){ return (m_mode==SHUTDOWN); }

private:
	char m_mode;
	CGameLauncher *mp_GameLauncher;
	CPassive *mp_PassiveMode;

	Uint8 m_Episode;
	Uint8 m_ChosenGame;
	std::string m_DataDirectory;
	int current_demo;

	CEGAGraphics *m_EGAGraphics;
	CMessages *m_Messages;
	CTileLoader *m_TileLoader;
};

#endif /* CGAMECONTROL_H_ */
