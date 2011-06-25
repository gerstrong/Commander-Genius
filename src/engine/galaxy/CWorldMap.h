/*
 * CWorldMap.h
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 *
 *  This is in charge of the ai and physics on the worldmap
 */

#ifndef CWORLDMAP_H_
#define CWORLDMAP_H_

#include "CMapPlayGalaxy.h"

namespace galaxy {

class CWorldMap : public CMapPlayGalaxy {
public:
	CWorldMap(CExeFile &ExeFile, CInventory &Inventory, stCheat &Cheatmode);

	void init();
	void loadAndPlayMusic();
};

}

#endif /* CWORLDMAP_H_ */
