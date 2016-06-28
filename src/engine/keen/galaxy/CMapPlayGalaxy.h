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

#include "engine/core/Cheat.h"
#include "common/CInventory.h"
#include "common/CGalaxySpriteObject.h"
//#include "engine/core/CBehaviorEngine.h"
#include "ep4/CMapLoaderGalaxyEp4.h"
#include "ep5/CMapLoaderGalaxyEp5.h"
#include "ep6/CMapLoaderGalaxyEp6.h"
#include <base/CInput.h>
//#include "sdl/CVideoDriver.h"
#include "sdl/audio/music/CMusic.h"
#include <base/utils/StringUtils.h>
#include <memory>
#include <vector>
#include <list>

class CMapPlayGalaxy
{
public:
    CMapPlayGalaxy(std::vector<CInventory> &inventoryVec);

    bool isActive() const
    {
        return mActive;
    }


	void setActive(const bool value);

	/*
	 * \brief Just tells the name of the level. Every Galaxy Level with a name
	 */
	std::string getLevelName();

    void pumpEvent(const CEvent *evPtr);

    /**
     * @brief ponderBase Think method
     * @param deltaT for this iteration how much time to think
     */
    void ponderBase(const float deltaT);

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
	std::vector<CInventory> &mInventoryVec;
	stOption *mpOption;

    bool mMsgBoxOpen;
};

#endif /* CMAPPLAYGALAXY_H_ */
