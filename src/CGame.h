/**
 * CGame.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef CGAME_H_
#define CGAME_H_

#include "core/CGameControl.h"
#include "sdl/CSettings.h"

class CGame {
public:
	CGame();
	
	bool init(int argc, char *argv[]);
	bool loadCKPDrivers();
	
	void run();
	void cleanup();

private:
	bool m_firsttime;
	CGameControl m_Engine;
	CSettings Settings();
};

#endif /* CGAME_H_ */
