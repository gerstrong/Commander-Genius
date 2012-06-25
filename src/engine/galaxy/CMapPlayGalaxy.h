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

#include "common/Cheat.h"
#include "fileio/CExeFile.h"
#include "engine/galaxy/CInventory.h"
#include "engine/galaxy/CGalaxySpriteObject.h"
#include "engine/galaxy/dialog/CMessageBoxGalaxy.h"
#include "common/CBehaviorEngine.h"
#include "CMapLoaderGalaxy.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "StringUtils.h"
#include "SmartPointer.h"
#include <vector>
#include <list>

class CMapPlayGalaxy {
public:
	CMapPlayGalaxy(CExeFile &ExeFile, CInventory &Inventory, stCheat &Cheatmode);

	bool isActive();
	void setActive(const bool value);

	/*
	 * \brief Just tells the name of the level. Every Galaxy Level with a name
	 */
	std::string getLevelName();

	void process(const bool msgboxactive);

	// Push data to Savegame-Controller
	void operator>>(CSaveGameController &savedGame);

	// This is for loading the game
	bool operator<<(CSaveGameController &savedGame);


protected:
	std::vector< SmartPointer<CGalaxySpriteObject> > mObjectPtr;
	bool mActive;

	CMap mMap;
	CExeFile &mExeFile;
	CInventory &mInventory;
	stOption *mpOption;
	stCheat &mCheatmode;
};

#endif /* CMAPPLAYGALAXY_H_ */
