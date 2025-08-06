/*
 * CMapPlayGalaxy.h
 *
 *  Created on: 25.03.2011
 *      Author: gerstrong
 *
 *  This is the play base class for Commander Keen Galaxy when the player
 *  has started the game. It is overloaded by CLevelPlay and CWorldMap to distinguish
 *  the play scenes.
 */

#ifndef CMAPPLAYGALAXY_H_
#define CMAPPLAYGALAXY_H_

#include "engine/core/Cheat.h"
#include "common/CInventory.h"
#include "common/CGalaxySpriteObject.h"
#include "ep4/CMapLoaderGalaxyEp4.h"
#include "ep5/CMapLoaderGalaxyEp5.h"
#include "ep6/CMapLoaderGalaxyEp6.h"

#include <base/CInput.h>
#include <base/audio/music/CMusic.h>
#include <base/interface/StringUtils.h>

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
    std::string getLevelName() const
    {
        return mpMap->getLevelName();
    }

    /**
     * @brief getLevelIdx
     * @return
     */
    Uint16 getLevelIdx() const
    {
        assert(mpMap);
        return mpMap->getLevel();
    }

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

    CSpriteObject *getNextStandingStillPlayer();

    void revivePlayerAt(const int playerIdx, const GsVec2D<int> pos);

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
    void operator>>(GsKit::ptree &levelNode);

    // This is for loading the game
    void operator<<(GsKit::ptree &levelNode);


    void setMsgBoxOpen(const bool msgboxactive)
    { mMsgBoxOpen = msgboxactive; }

    void stopMusic();

    void playMusic(const int track);

    void reloadBgMusic();

protected:
    std::vector< std::shared_ptr<CGalaxySpriteObject> > mObjectPtr;
    bool mActive = false;

    std::shared_ptr<CMap> mpMap;
    std::vector<CInventory> &mInventoryVec;

    bool mMsgBoxOpen = false;

    std::string mCurMusicTrack = "-1";
};

#endif /* CMAPPLAYGALAXY_H_ */
