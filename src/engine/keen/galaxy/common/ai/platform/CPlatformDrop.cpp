/*
 * CPlatformDrop.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformDrop.h"

// If the max Speed is reached, the platform won't return.
const int DROP_MAX_SPEED_LIMIT = 65;

const int BLOCKSPRITEID = 0x1F;

namespace galaxy {

CPlatformDrop::CPlatformDrop(CMap *pmap, const Uint16 foeID, 
                 const Uint32 x, const Uint32 y, const int actionOff, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
CPlatform(pmap, foeID, x, y),
m_delay_for_drop(0),
m_drop_speed(0),
m_Origin(m_Pos),
mAllowReturn(false)
{
	m_ActionBaseOffset = actionOff;
	xDirection = 0;
	yDirection = 0;
    solid = false;

    dropSpeedAcc = 5;
    hoverSpeed = DROP_MAX_SPEED_LIMIT;

    auto diff = g_pBehaviorEngine->mDifficulty;

    if(diff > NINJA)
    {
        dropSpeedAcc = 10;
        hoverSpeed = DROP_MAX_SPEED_LIMIT;
    }
    if(diff > EXPERT)
    {
        dropSpeedAcc = 5;
        hoverSpeed = DROP_MAX_SPEED_LIMIT;
    }
    if(diff > HARD)
    {
        dropSpeedAcc = 5;
        hoverSpeed = DROP_MAX_SPEED_LIMIT/2;
    }


	setActionForce(A_PLATFORM_DROP);
	setActionSprite();
	calcBoundingBoxes();

    // Decide whether plat can return or must fall forever
    Vector2D<Uint32> blockerPos = getPosition();

    // Scan the map for blockers, if there is one, it may return, otherwise not...
    for( ; blockerPos.y < (mp_Map->m_height<<CSF) ; blockerPos.y += (1<<CSF) )
    {
        const Uint16 object = mp_Map->getPlaneDataAt(2, blockerPos);
        if( object == BLOCKSPRITEID )
        {
            mAllowReturn = true;
            break;
        }
    }

}

void CPlatformDrop::procPlatdrop()
{
    // move down
    movePlatDown(m_drop_speed);

    // If speed is lower than max increase it...
    if(m_drop_speed < DROP_MAX_SPEED_LIMIT)
        m_drop_speed += dropSpeedAcc;
    else // else set the max speed
        m_drop_speed = DROP_MAX_SPEED_LIMIT;
}


void CPlatformDrop::process()
{    
    const Uint16 object = mp_Map->getPlaneDataAt(2, getPosition());
	
	bool blockerDetected = false;
	
	// If there is a blocker, change the direction
	if( object == BLOCKSPRITEID )
	    blockerDetected = true;

	bool drop = false;	
    for( auto &player : mCarriedPlayerVec)
	{
        if(!player)
            continue;

        if(!player->m_jumpdownfromobject)
	    {
            drop = true;
            break;
	    }
	}
	
	// Player is standing on the platform or the platform is already falling too fast
	if(drop)
	{
        if(!blockerDetected)
        {
            procPlatdrop();
        }
	}
    else if(mAllowReturn) // Player is not on the platform and may return
	{        
		m_delay_for_drop = 0;
		m_drop_speed = 0;

        if(m_Origin.y < m_Pos.y)
            moveUp(hoverSpeed);
		else if(m_Origin.y > m_Pos.y)
			moveDown(1);
	}
    else if(m_drop_speed > 0) // not even a blocker. continue moving until out of map.
    {
        procPlatdrop();
    }

	CPlatform::process();
}

}
