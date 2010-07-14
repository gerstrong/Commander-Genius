/*
 * CPlayGame.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPLAYGAMEGALAXY_H_
#define CPLAYGAMEGALAXY_H_

#include "../playgame/CPlayGame.h"
#include "../../common/CMap.h"
#include "CWorldMap.h"
#include <vector>

namespace galaxy
{

class CPlayGameGalaxy : public CPlayGame
{
public:
	CPlayGameGalaxy(CExeFile &ExeFile, char level,
			 char numplayers, char difficulty,
			 stOption *p_option);

	bool loadGameState();
	void loadLevel();
	bool init();

	void process();

	void processInput();
	void processRendering();

	void cleanup();

	virtual ~CPlayGameGalaxy();

private:
	CWorldMap m_WorldMap;

};

}

#endif /* CPLAYGAMEGALAXY_H_ */
