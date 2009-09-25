/*
 * CGame.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef CGAME_H_
#define CGAME_H_

#include "CGameControl.h"

// references to cinematics/CommonEnding
//int endsequence(stCloneKeenPlus *pCKP);

// Reference from game.cpp
//void initgame(stLevelControl *p_levelcontrol);

class CGame {
public:
	CGame();
	virtual ~CGame();

	bool init();
	void run();
	void cleanup();

	///////
	/// Depreciated
	///////
	/*void freeResources(void);
	short runCycle(stCloneKeenPlus *pCKP);
	short ingamerunCycle(stCloneKeenPlus *pCKP);
	void preallocateCKP(stCloneKeenPlus *pCKP);*/

private:
	CGameControl m_GameControl;
};

#endif /* CGAME_H_ */
