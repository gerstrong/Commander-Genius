/*
 * CAmpton.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CFuse.h"
#include "engine/galaxy/common/ai/CPlayerLevel.h"
#include "misc.h"

/*
 $3186W #QED?
 */


namespace galaxy {
    
    
    CFuse::CFuse(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
    CGalaxySpriteObject(pmap, foeID, x, y),
    mTimer(0)
    {
        // Adapt this AI
        setupGalaxyObjectOnMap(0x3186, 0);
        
        xDirection = LEFT;
        
        mp_Map->mNumFuses++;
        mp_Map->mFuseInLevel = true;
    }
    
    
    void CFuse::getTouchedBy(CSpriteObject &theObject)
    {
        if(dead || theObject.dead)
            return;
        
        /*if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
         {
         player->kill();
         }*/
    }
    
    
    void CFuse::process()
    {
        if(!processActionRoutine())
            exists = false;
    }
    
}
