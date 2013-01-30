/*
 * CLevelPlay.h
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 *
 *  This class will manage the entire gameplay in one level
 */

#ifndef CLEVELPLAY_H_
#define CLEVELPLAY_H_

#include "CMapPlayGalaxy.h"

namespace galaxy {

class CLevelPlay : public CMapPlayGalaxy
{
public:
	CLevelPlay(CExeFile &ExeFile, CInventory &Inventory, stCheat &Cheatmode);

	void loadMap(const int level);
	void reloadLevel();

	bool loadLevel(const Uint16 level);
};

}

#endif /* CLEVELPLAY_H_ */
