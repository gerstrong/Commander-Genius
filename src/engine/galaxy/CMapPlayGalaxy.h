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

#include "common/CSpriteObject.h"
#include "common/Cheat.h"
#include "fileio/CExeFile.h"
#include "engine/galaxy/CInventory.h"
#include "engine/galaxy/dialog/CMessageBoxGalaxy.h"
#include "common/CBehaviorEngine.h"
#include "CMapLoaderGalaxy.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "StringUtils.h"
#include <vector>
#include <list>

class CMapPlayGalaxy {
public:
	CMapPlayGalaxy(CExeFile &ExeFile, CInventory &Inventory, stCheat &Cheatmode);
	~CMapPlayGalaxy();

	bool isActive();
	void setActive(const bool value);

	/*
	 * \brief Just tells the name of the level. Every Galaxy Level with a name
	 */
	std::string getLevelName();

	void process(const bool msgboxactive);

protected:
	std::vector<CSpriteObject*> m_ObjectPtr;
	bool m_active;

	CMap m_Map;
	CExeFile &m_ExeFile;
	CInventory &m_Inventory;
	stOption *mp_option;
	stCheat &m_Cheatmode;
};

#endif /* CMAPPLAYGALAXY_H_ */
