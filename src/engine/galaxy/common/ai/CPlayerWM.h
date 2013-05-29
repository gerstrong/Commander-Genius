/*
 * CPlayer.h
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#ifndef CPLAYERWM_H_
#define CPLAYERWM_H_

#include "CPlayerBase.h"

namespace galaxy {

class CPlayerWM : public CPlayerBase
{
public:
	CPlayerWM(CMap *pmap,
			const Uint16 foeID,
			Uint32 x,
			Uint32 y,
			CInventory &l_Inventory,
			stCheat &Cheatmode,
			const unsigned int actionoffset);

	VectorD2<int> fetchFootDestCoord();
	
	void deserialize(CSaveGameController &savedGame) 
	{
	  savedGame.decodeData(mUsedGrapplingHook);
	}

	void serialize(CSaveGameController &savedGame) 
	{
	  savedGame.encodeData(mUsedGrapplingHook);
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

    void setupTeleportAnimation(const bool unset, const VectorD2<int> &pos);
	void processEnteringTeleporter();
	void processWarpInTeleporter();
	void processLeavingTeleporter();

	void startLevel(Uint16 object);
	void finishLevel(const int object);
	
	// Checks if Keen in CK6 can climb
	bool checkforClimbing(direction_t &climbDir);
	void checkforSwimming(bool &bleft, bool &bright, bool &bup, bool &bdown);

	void performWalkingAnimation(bool walking);
	void performSwimmingAnimation();
	
	void setMounted(const bool value);

private:
	Uint16 m_basesprite;
	Uint16 m_teleportanibasetile;
    Uint16 m_teleportoldtile;
	Uint16 walkBaseFrame;
	Uint16 climbBaseFrame;
	Uint16 wavingBaseFrame;
	Uint16 swimBaseFrame;
	direction_t m_looking_dir;

	Uint8 m_animation;
	Uint8 m_animation_time;
	Uint8 m_animation_ticker;
	bool m_cantswim;
	VectorD2<int> target;
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
