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
        
        void process();
        void processMoving();
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
        
        void processEnteringTeleporter();
        void processWarpInTeleporter();
        void processLeavingTeleporter();
        
        void startLevel(Uint16 object);
        void finishLevel(Uint16 object);
        void checkforSwimming(bool &bleft, bool &bright, bool &bup, bool &bdown);
        
        void performWalkingAnimation(bool walking);
        void performSwimmingAnimation();
        
    private:
        Uint16 m_basesprite;
        Uint16 m_teleportanibasetile;
        Uint16 walkBaseFrame;
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
        
        std::vector<bool> mLevelDone;
        
        void (CPlayerWM::*mProcessPtr)();
    };
    
}

#endif /* CPLAYERWM_H_ */
