/*
 * CGame.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef CGAME_H_
#define CGAME_H_

#include "CGameControl.h"
#include "common/options.h"

// references to cinematics/CommonEnding
//int endsequence(stCloneKeenPlus *pCKP);

// Reference from game.cpp
//void initgame(stLevelControl *p_levelcontrol);

class CGame {
public:
	CGame();
	virtual ~CGame();
	
	bool init(int argc, char *argv[]);
	bool loadCKPDrivers();
	
	void run();
	void cleanup();
	
	stOption m_option[NUM_OPTIONS];
	
private:
	CGameControl m_GameControl;
};

#endif /* CGAME_H_ */
