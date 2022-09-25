/*
 * CPlayer.h
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERWM_H_
#define CPLAYERWM_H_

#include "CPlayerBase.h"


struct EventPlayerEndLevel : CEvent {
    EventPlayerEndLevel(const EventExitLevel &ev) :
                    levelObject(ev.levelObject),
                    sucess(ev.sucess),
                    who(ev.who),
                    teleport(ev.teleport){}

    const uint16_t levelObject;
    const bool sucess;
    const int who;
    const bool teleport;

};

struct EventPlayerRideFoot : CEvent {
    EventPlayerRideFoot(const EventExitLevelWithFoot &ev) :
                    levelObject(ev.levelObject),
                    who(ev.who) {}

    const uint16_t levelObject;
    const int who;

};


namespace galaxy {

class CPlayerWM : public CPlayerBase
{
public:
    CPlayerWM(CMap *pmap,
            const Uint16 foeID,
            Uint32 x,
            Uint32 y,
            CInventory &l_Inventory,
            const unsigned int actionoffset,
            const int playerIdx,
            const int spriteTableIdx);

    GsVec2D<int> fetchFootDestCoord();

    void pumpEvent(const std::shared_ptr<CEvent> &evPtr);

    void deserialize(CSaveGameController &savedGame);
    void serialize(CSaveGameController &savedGame);

    void serialize(GsKit::ptree &node);
    void deserialize(GsKit::ptree &node);

    void process();
    void processMoving();
    void processClimbing();
    void processWaving();

    // Episode 4 only. When Keen is riding on the foot
    void processRiding();

    // Episode 5 and 6: Those processes make Keen walk through elevators and teleporters
    void verifyTeleportation();
    void processEnteringElevator();
    void processClosingElevator();
    void processElevating();
    void processOpeningElevator();
    void processLeavingElevator();

    void setupTeleportAnimation(const bool unset, const GsVec2D<int> &pos);
    void processEnteringTeleporter();
    void processWarpInTeleporter();
    void processLeavingTeleporter();

    void startLevel(Uint16 object);
    bool finishLevel(const int object);

    // Checks if Keen in CK6 can climb
    bool checkforClimbing(direction_t &climbDir);
    void checkforSwimming(bool &bleft, bool &bright, bool &bup, bool &bdown);

    void performWalkingAnimation(bool walking);
    void performSwimmingAnimation(const bool moving);

    void setMounted(const bool value);

    void processMoveBitDown();

    bool isSwimming();
    void makeHimSwim(const bool value);

private:
    Uint16 m_basesprite = 0;
    Uint16 m_teleportanibasetile = 0;
    Uint16 m_teleportoldtile = 0;
    Uint16 walkBaseFrame = 0;
    Uint16 climbBaseFrame;
    Uint16 wavingBaseFrame;
    Uint16 swimBaseFrame;

    Uint8 m_animation = 0;
    Uint8 m_animation_time = 1;
    Uint8 m_animation_ticker = 0;
    bool m_cantswim = false;
    GsVec2D<int> target;
    int elevator_close_timer = 0;
    int elevator_frames = 0;
    int waveTimer = 0;
    bool swimming = false;
    bool mUsedGrapplingHook = false;
    bool mounted = false;

    std::vector<bool> mLevelDone;

    void (CPlayerWM::*mProcessPtr)() = nullptr;
};

}

#endif /* CPLAYERWM_H_ */
