/*
 * CPlayGame.h
 *
 *  Created on: 31.01.2010
 *      Author: gerstrong
 */

#ifndef CPLAYGAMEGALAXY_H_
#define CPLAYGAMEGALAXY_H_

#include "engine/core/mode/CPlayGame.h"
#include "common/CInventory.h"
#include "common/dialog/CMessageBoxGalaxy.h"
//#include "engine/core/CMap.h"
#include "common/CWorldMap.h"
#include "CLevelPlay.h"

#include <graphics/GsBitmap.h>

#include <memory>
#include <vector>
#include <list>
#include <string>

const int WORLD_MAP_LEVEL_GALAXY = 0;

namespace galaxy
{

class CPlayGameGalaxy : public CPlayGame
{
public:

    CPlayGameGalaxy(const int startlevel,
                    const std::vector<int> &spriteVars);

    bool loadXMLGameState();
    bool saveXMLGameState();
    bool init() override;

    void looseManagement(const int playerID,
                         const uint16_t levelObj,
                         const std::string &levelName);

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

    void ponder(const float deltaT);
    void render();

    void processInput();
    void processRendering();

    void operator<<(GsKit::ptree &invNode);

    void cleanup() {}

private:
    std::vector<CInventory> mInventoryVec;
    CWorldMap m_WorldMap;
    CLevelPlay m_LevelPlay;
    GsBitmap m_BackgroundBitmap;
    std::list< std::shared_ptr<CMessageBoxGalaxy> > mMessageBoxes;

    std::vector<bool> mDead;
    std::vector<bool> mGameOver;

    // Menu-Button
    SDL_Rect mMenuButtonRect = {0, 0, 24, 16};

    bool mGameOverTriggered = false;
};

}

#endif /* CPLAYGAMEGALAXY_H_ */
