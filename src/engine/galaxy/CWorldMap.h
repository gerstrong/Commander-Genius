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

#include "common/CObject.h"
#include "fileio/CExeFile.h"
#include "engine/galaxy/CInventory.h"
#include "common/CBehaviorEngine.h"
#include <vector>

namespace galaxy {

class CWorldMap {
public:
	CWorldMap(CExeFile &ExeFile, CInventory &Inventory);
	bool isActive();
	void setActive(bool value);

	void init();
	std::string getLevelName();
	void loadAndPlayMusic();
	void process();

	virtual ~CWorldMap();

private:
	std::vector<CObject*> m_ObjectPtr;
	bool m_active;

	CMap m_Map;
	CExeFile &m_ExeFile;
	CInventory &m_Inventory;
	stOption *mp_option;
};

}

#endif /* CWORLDMAP_H_ */
