/*
 * CBullet.cpp
 *
 *  Created on: 01.12.2010
 *      Author: gerstrong
 */

#include "CBullet.h"
#include "sdl/sound/CSound.h"

size_t bulletActionMap[] =
{	0x1738, 0x1502,0x153E	};

namespace galaxy
{
    
    const Uint32 speed = 10;
    const Uint32 hittime = 100;
    
    // Test
    //int slot = 64;
    
    CBullet::CBullet(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xDir, const int yDir) :
    CGalaxySpriteObject(pmap, foeID, x, y)
    {
        xDirection = xDir;
        yDirection = yDir;
        
        const size_t ep = g_pBehaviorEngine->getEpisode();
        
        setupGalaxyObjectOnMap(bulletActionMap[ep-4], A_KEENSHOT_MOVING);
        setActionSprite();
        calcBoundingBoxes();
        playSound( SOUND_KEEN_FIRE );
        
        // For testing sounds.
        /*g_pSound->playStereosoundSlot(slot, PLAY_NOW, 0);
         printf("Sound %d played!\n", slot-64);
         slot++;*/
    }
    
    void CBullet::process()
    {
        /*int trans = transluceny;
         
         if( trans > 30 )
         trans -= 10;
         else
         trans = 30;
         
         transluceny = trans;*/
        
        
        if( !getActionNumber(A_KEENSHOT_IMPACT) && (blockedd || blockedu || blockedl || blockedr || onslope) )
        {
            setAction(A_KEENSHOT_IMPACT);
            playSound( SOUND_SHOT_HIT );
            dead = true;
        }
        
        if(!processActionRoutine())
			exists = false;
        
    }
    
}
