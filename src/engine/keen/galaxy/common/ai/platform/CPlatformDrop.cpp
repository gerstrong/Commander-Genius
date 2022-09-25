/*
 * CPlatformDrop.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformDrop.h"
#include <engine/core/CBehaviorEngine.h>

// If the max Speed is reached, the platform won't return.
const int DROP_MAX_SPEED_LIMIT = 65;

const int BLOCKSPRITEID = 0x1F;
//const int START_DELAY_FOR_DROP = 6;
const int START_DELAY_FOR_DROP = 12;

namespace galaxy {

CPlatformDrop::CPlatformDrop(CMap *pmap, const Uint16 foeID, 
                 const Uint32 x, const Uint32 y, const int actionOff, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
CPlatform(pmap, foeID, x, y),
m_Origin(m_Pos)
{    
    m_delay_for_drop = START_DELAY_FOR_DROP;

	m_ActionBaseOffset = actionOff;

    dropSpeedAcc = 5;
    hoverSpeed = DROP_MAX_SPEED_LIMIT;

    auto diff = gBehaviorEngine.mDifficulty;

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
    GsVec2D<Uint32> blockerPos = getPosition();

    // Scan the map for blockers, if there is one, it may return, otherwise not...
    for( ; blockerPos.y < (mpMap->m_height<<CSF) ; blockerPos.y += (1<<CSF) )
    {
        const Uint16 object = mpMap->getPlaneDataAt(2, blockerPos);
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
    const Uint16 object = mpMap->getPlaneDataAt(2, getPosition());
	
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
            if(m_delay_for_drop > 0)
                m_delay_for_drop--;
            drop = true;
            break;
	    }
	}
	
	// Player is standing on the platform or the platform is already falling too fast
    if(drop)
	{
        if( m_delay_for_drop == 0 )
        {
            if(!blockerDetected)
            {
                procPlatdrop();
            }
        }
        else
        {
            movePlatDown(DROP_MAX_SPEED_LIMIT/2);
        }
	}
    else if(mAllowReturn) // Player is not on the platform and may return
	{        
        m_delay_for_drop = START_DELAY_FOR_DROP;
		m_drop_speed = 0;

        if(m_Origin.y < m_Pos.y)
        {
            moveUp(hoverSpeed);
        }
		else if(m_Origin.y > m_Pos.y)
        {
			moveDown(1);
        }
	}
    else if(m_drop_speed > 0) // not even a blocker. continue moving until out of map.
    {
        procPlatdrop();
    }

	CPlatform::process();
}

void CPlatformDrop::deserialize(GsKit::ptree &node)
{
    try
    {
        mAllowReturn = node.get<bool>("<xmlattr>.allowReturn", mAllowReturn);
        auto &posNode = node.put("pos", "");
        m_Pos.x = posNode.get<Uint32>("<xmlattr>.x", m_Pos.x);
        m_Pos.y = posNode.get<Uint32>("<xmlattr>.y", m_Pos.y);
        auto &originNode = node.put("origin", "");
        m_Origin.x = originNode.get<Uint32>("<xmlattr>.x", m_Origin.x);
        m_Origin.y = originNode.get<Uint32>("<xmlattr>.y", m_Origin.y);
    }
    catch(...)
    {
        gLogging << "Some data is missing in the save." << CLogFile::endl;
    }
}

void CPlatformDrop::serialize(GsKit::ptree &node)
{
    node.put("<xmlattr>.allowReturn", mAllowReturn);
    auto &posNode = node.put("pos", "");
    posNode.put("<xmlattr>.x", m_Pos.x);
    posNode.put("<xmlattr>.y", m_Pos.y);
    auto &originNode = node.put("origin", "");
    originNode.put("<xmlattr>.x", m_Origin.x);
    originNode.put("<xmlattr>.y", m_Origin.y);
}


}
