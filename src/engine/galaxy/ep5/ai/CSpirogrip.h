/*
 * CSpirogrip.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Spirogrips in Galaxy
 */

#ifndef CSPIROGRIP_H_
#define CSPIROGRIP_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {
    
    
    class CSpirogrip : public CStunnable
    {
    public:
        CSpirogrip(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
        
        /**
         * Action functions
         */
        void processBackup();
        void processPause();
        void processSpin();
        void processMove();
        
        bool isNearby(CSpriteObject &theObject);
        
        void process();
        
        /**
         * What happens if the slug gets touched by another object
         */
        void getTouchedBy(CSpriteObject &theObject);
        
    private:
        int mTimer;
        direction_t mKeenAlignmentX;
        direction_t mKeenAlignmentY;
    };
    
}

#endif /* CSPIROGRIP_H_ */
