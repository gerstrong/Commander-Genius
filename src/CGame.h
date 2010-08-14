/**
 * CGame.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef CGAME_H_
#define CGAME_H_

#include "CGameControl.h"
#include "common/options.h"
#include "sdl/CSettings.h"

class CGame {
public:
	CGame();
	
	bool init(int argc, char *argv[]);
	bool loadCKPDrivers();
	
	void run();
	void cleanup();
	
	stOption m_option[NUM_OPTIONS];
	
private:
	bool m_firsttime;
	CGameControl m_Engine;
	CSettings Settings();
};

#endif /* CGAME_H_ */
