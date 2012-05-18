/*
 * CPlayGame.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPLAYGAMEGALAXY_H_
#define CPLAYGAMEGALAXY_H_

#include "engine/playgame/CPlayGame.h"
#include "engine/galaxy/CInventory.h"
#include "dialog/CMessageBoxGalaxy.h"
#include "common/CMap.h"
#include "CWorldMap.h"
#include "CLevelPlay.h"
#include "CMenuGalaxy.h"
#include "SmartPointer.h"
#include <vector>
#include <list>
#include <string>

const int WORLD_MAP_LEVEL_GALAXY=0;

namespace galaxy
{

class CPlayGameGalaxy : public CPlayGame
{
public:
	CPlayGameGalaxy(CExeFile &ExeFile, char level,
			 char numplayers,
			  CSaveGameController &SavedGame);

	bool loadGameState();
	bool saveGameState();
	bool init();

	void process();

	void processInput();
	void processRendering();

	void cleanup() {};

private:
	std::string m_LevelName;
	CInventory m_Inventory;
	CWorldMap m_WorldMap;
	CLevelPlay m_LevelPlay;
	CSaveGameController &m_SavedGame;
	CBitmap m_BackgroundBitmap;
	std::list< SmartPointer<CMessageBoxGalaxy> > mMessageBoxes;
	stCheat m_Cheatmode;
};

}

#endif /* CPLAYGAMEGALAXY_H_ */
