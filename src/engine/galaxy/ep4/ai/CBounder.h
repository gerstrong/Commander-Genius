/*
 * CBounder.h
 *
 *  Created on: 02.07.2011
 *      Author: gerstrong
 */

#ifndef CBOUNDER_H_
#define CBOUNDER_H_

#include "engine/galaxy/common/ai/CStunnable.h"
#include "engine/galaxy/common/ai/CPlayerLevel.h"

namespace galaxy {
    
    class CBounder : public CStunnable
    {
    public:
        CBounder(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);
        
        void getTouchedBy(CSpriteObject &theObject);
        void processBounce();
        void processOnFloor();
        
        void movePlatLeft(const int amnt);
        void movePlatRight(const int amnt);
        void movePlayerUp(const int amnt);
        void movePlayerDown(const int amnt);
        
        void moveLeft(const int amnt, const bool force);
        void moveRight(const int amnt, const bool force);
        void moveUp(const int amnt);
        void moveDown(const int amnt);
        
        void process();
        
    private:
        int bounceboost;
        
        CPlayerLevel *mpInteractPlayer;
    };
    
};

#endif /* CBOUNDER_H_ */
