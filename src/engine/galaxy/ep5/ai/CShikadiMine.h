/*
 * CShikadiMine.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Shikadi Mine in Galaxy
 */

#ifndef CSHIKADIMINE_H_
#define CSHIKADIMINE_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {
    
    
    class CShikadiMine : public CStunnable
    {
    public:
        CShikadiMine(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);
        
        /**
         * Action functions
         */
        void processSit();
        void processMoving();
        void processChangeDir();
        void processDetonate();
        
        void setEyeCenterOffset(int &x, int &y);
        
        bool isNearby(CSpriteObject &theObject);
        
        void process();
        
        void draw();
        
        /**
         * What happens if the foe gets touched by another object
         */
        void getTouchedBy(CSpriteObject &theObject);
        
    private:
        int mTimer;
        int mEyeSprite;
        int mEyeXOffset;
        int mEyeYOffset;
        int mTargetEyeXOffset;
        int mTargetEyeYOffset;
        direction_t mKeenAlignmentX;
        direction_t mKeenAlignmentY;
    };
    
    
    ///////////////////
    /// Mine shards ///
    ///////////////////
    
    class CMineShards : public CStunnable
    {
        
    public:
        CMineShards(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xSpeed);
        
        void process();
        
        /**
         * What happens if the foe gets touched by another object
         */
        void getTouchedBy(CSpriteObject &theObject);    
        
    private:
        int mXSpeed;    
        
    };
    
    
}

#endif /* CSHIKADIMINE_H_ */
