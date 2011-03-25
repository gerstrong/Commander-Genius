/*
 * CMapPlayGalaxy.h
 *
 *  Created on: 25.03.2011
 *      Author: gerstrong
 *
 *  This is the play base class for Commander Keen Galaxy when the player
 *  has started the game. It is overloaded with CLevelPlay and CWorldMap to distinguish
 *  the play scenes. If those classes get to small due the common stuff they might get reduced
 */

#ifndef CMAPPLAYGALAXY_H_
#define CMAPPLAYGALAXY_H_

#include "common/CObject.h"
#include "fileio/CExeFile.h"
#include "engine/galaxy/CInventory.h"
#include "common/CBehaviorEngine.h"
#include "CMapLoaderGalaxy.h"
#include "sdl/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "StringUtils.h"
#include <vector>


class CMapPlayGalaxy {
public:
	CMapPlayGalaxy(CExeFile &ExeFile, CInventory &Inventory);
	~CMapPlayGalaxy();

	bool isActive();
	void setActive(const bool value);

	std::string getLevelName();

	void process();

protected:
	std::vector<CObject*> m_ObjectPtr;
	bool m_active;

	CMap m_Map;
	CExeFile &m_ExeFile;
	CInventory &m_Inventory;
	stOption *mp_option;
};

#endif /* CMAPPLAYGALAXY_H_ */
