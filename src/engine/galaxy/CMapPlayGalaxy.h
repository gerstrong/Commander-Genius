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
#include "engine/galaxy/common/CInventory.h"
#include "engine/galaxy/common/CGalaxySpriteObject.h"
#include "common/CBehaviorEngine.h"
#include "ep4/CMapLoaderGalaxyEp4.h"
#include "ep5/CMapLoaderGalaxyEp5.h"
#include "ep6/CMapLoaderGalaxyEp6.h"
#include "sdl/input/CInput.h"
#include "sdl/CVideoDriver.h"
#include "sdl/music/CMusic.h"
#include "core/LogicDrawSplitter.h"
#include "StringUtils.h"
#include <memory>
#include <vector>
#include <list>

class CMapPlayGalaxy : public GameState
{
public:
    CMapPlayGalaxy(CExeFile &ExeFile, std::vector<CInventory> &inventoryVec, stCheat &Cheatmode);

	bool isActive();
	void setActive(const bool value);

	/*
	 * \brief Just tells the name of the level. Every Galaxy Level with a name
	 */
	std::string getLevelName();

    void ponder();

    void render();

	// Push data to Savegame-Controller
	void operator>>(CSaveGameController &savedGame);

	// This is for loading the game
	bool operator<<(CSaveGameController &savedGame);

    // Saves the inventory using the Savegamecontroller.
    void operator>>(boost::property_tree::ptree &levelNode);

    // This is for loading the game
    void operator<<(boost::property_tree::ptree &levelNode);


	CMap &getMapObj()
	{	return mMap	;}


    void setMsgBoxOpen(const bool msgboxactive)
    { mMsgBoxOpen = msgboxactive; }

protected:
	std::vector< std::shared_ptr<CGalaxySpriteObject> > mObjectPtr;
	bool mActive;        

	CMap mMap;
	CExeFile &mExeFile;
	std::vector<CInventory> &mInventoryVec;
	stOption *mpOption;
	stCheat &mCheatmode;

    bool mMsgBoxOpen;
};

#endif /* CMAPPLAYGALAXY_H_ */
