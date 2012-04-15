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
#include <vector>
#include <string>

const int WORLD_MAP_LEVEL_GALAXY=0;

namespace galaxy
{

class CPlayGameGalaxy : public CPlayGame
{
public:
	CPlayGameGalaxy(CExeFile &ExeFile, char level,
			 char numplayers, Uint8& difficulty,
			  CSaveGameController &SavedGame);

	bool loadGameState();
	bool saveGameState();
	bool init();

	void process();

	void processInput();
	void processRendering();

	void cleanup() {};

	virtual ~CPlayGameGalaxy();

private:
	std::string m_LevelName;
	CInventory m_Inventory;
	CWorldMap m_WorldMap;
	CLevelPlay m_LevelPlay;
	CMenuGalaxy *mp_Menu;
	CSaveGameController &m_SavedGame;
	CBitmap m_BackgroundBitmap;
	std::list<CMessageBoxGalaxy*> m_MessageBoxes;
	stCheat m_Cheatmode;
};

}

#endif /* CPLAYGAMEGALAXY_H_ */
