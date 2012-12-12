/*
 * CPlatformDrop.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformDrop.h"

// If the max Speed is reached, the platform won't return.
//const int DROP_MAX_SPEED_LIMIT = 200;
const int DROP_MAX_SPEED_LIMIT = 20;

// Times the cycle has to run through until Speed is raised
const int DROP_SPEED_ACC = 5;

// Speed at what the Platform might return to the original position
const int HOVER_SPEED = 25;

namespace galaxy {

CPlatformDrop::CPlatformDrop(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
CPlatform(pmap, foeID, x, y),
m_delay_for_drop(0),
m_drop_speed(0),
m_Origin(m_Pos),
blockerDetected(false)
{
	xDirection = 0;
	yDirection = 0;
	solid = false;
	setActionForce(A_PLATFORM_DROP);
	setActionSprite();
	calcBoundingBoxes();
}

void CPlatformDrop::process()
{    
	Uint16 object = mp_Map->getPlaneDataAt(2, getPosition());
	
	// If there is a blocker, change the direction
	if( object == 0x1F )
	{
	    blockerDetected = true;
	}
	else 
	{
	    blockerDetected = false;
	}
	
	
	// Player is standing on the platform or the platform is already falling too fast
	if(mp_CarriedPlayer /*|| m_drop_speed>=DROP_MAX_SPEED_LIMIT*/ || blockerDetected )
	{
		// move down
		movePlatDown(m_drop_speed);

		// If speed is lower than max increase it...
		if(m_drop_speed < DROP_MAX_SPEED_LIMIT)
			m_drop_speed += DROP_SPEED_ACC;
		else // else set the max speed
			m_drop_speed = DROP_MAX_SPEED_LIMIT;
	}
	else // Player is not on the platform
	{
		m_delay_for_drop = 0;
		m_drop_speed = 0;

		if(m_Origin.y < m_Pos.y)
			moveUp(HOVER_SPEED);
		else if(m_Origin.y > m_Pos.y)
			moveDown(1);
	}

	CPlatform::process();
}

}
