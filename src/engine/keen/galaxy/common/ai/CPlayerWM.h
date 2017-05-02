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
    const uint16_t levelObject;
    const bool sucess;
    const int who;
    const bool teleport;
    EventPlayerEndLevel(const EventExitLevel &ev) :
                    levelObject(ev.levelObject),
                    sucess(ev.sucess),
                    who(ev.who),
                    teleport(ev.teleport){}
};

struct EventPlayerRideFoot : CEvent {
    const uint16_t levelObject;
    EventPlayerRideFoot(const EventExitLevelWithFoot &ev) :
                    levelObject(ev.levelObject) {}
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

    Vector2D<int> fetchFootDestCoord();

    void pumpEvent(const CEvent *evPtr);
	
	void deserialize(CSaveGameController &savedGame) 
	{
	  savedGame.decodeData(mUsedGrapplingHook);
	}

	void serialize(CSaveGameController &savedGame) 
	{
	  savedGame.encodeData(mUsedGrapplingHook);                  
	}

    void serialize(boost::property_tree::ptree &node)
    {
        node.put("usedGrapplingHook",mUsedGrapplingHook);

        const bool swimming = isSwimming();
        node.put("isSwimming",swimming);
    }
    void deserialize(boost::property_tree::ptree &node)
    {
        mUsedGrapplingHook = node.get<bool>("usedGrapplingHook", false);

        bool swimming;
        swimming = node.get<bool>("isSwimming",false);
        makeHimSwim(swimming);
    }

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

    void setupTeleportAnimation(const bool unset, const Vector2D<int> &pos);
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
	Uint16 m_basesprite;
	Uint16 m_teleportanibasetile;
    Uint16 m_teleportoldtile;
	Uint16 walkBaseFrame;
	Uint16 climbBaseFrame;
	Uint16 wavingBaseFrame;
	Uint16 swimBaseFrame;

	Uint8 m_animation;
	Uint8 m_animation_time;
	Uint8 m_animation_ticker;
	bool m_cantswim;
    Vector2D<int> target;
	int elevator_close_timer;
	int elevator_frames;
	int waveTimer;
	bool swimming;
	bool mUsedGrapplingHook;
	bool mounted;

	std::vector<bool> mLevelDone;	

	void (CPlayerWM::*mProcessPtr)();
};

}

#endif /* CPLAYERWM_H_ */
