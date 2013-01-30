/**
 * CGame.h
 *
 *  Created on: 01.05.2009
 *      Author: gerstrong
 */

#ifndef CGAME_H_
#define CGAME_H_

#include "core/CGameControl.h"

class CGame {
public:
	CGame();
	~CGame();
	
	bool init(int argc, char *argv[]);
	bool loadCKPDrivers();
	
	void run();
	void cleanup();

private:
	bool m_firsttime;
	CGameControl m_Engine;
};

#endif /* CGAME_H_ */
