/*
 * CPlatformMoveAway.cpp
 *
 *  Created on: 11.09.2012
 *      Author: gerstrong
 */

#include "CPlatformMoveAway.h"

// If the max Speed is reached, the platform won't return.
const int DROP_MAX_SPEED_LIMIT = 200;

// Times the cycle has to run through until Speed is raised
const int DROP_SPEED_ACC = 5;

// Speed at what the Platform might return to the original position
const int HOVER_SPEED = 25;

// The distance to get the platform moved
const int CSF_DISTANCE_TO_FOLLOW_GETAWAY = 1<<CSF;

// The distance to get the platform moved
const int CSF_DISTANCE_TO_GETBACK = 10<<CSF;


namespace galaxy {

CPlatformMoveAway::CPlatformMoveAway(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
		const direction_t vertdir, const direction_t hordir, const int actionOffset) :
CGalaxySpriteObject(pmap, foeID, x, y),
CPlatform(pmap, foeID, x, y),
m_Origin(m_Pos)
{
	xDirection = hordir;
	yDirection = vertdir;
	solid = false;
	m_ActionBaseOffset = actionOffset;
	setActionForce(A_PLATFORM_DROP);
	mpActionProc = &CPlatformMoveAway::processStay;
	setActionSprite();
	calcBoundingBoxes();
}


bool CPlatformMoveAway::isNearby(CSpriteObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		const int dx = player->getXMidPos() - getXMidPos();
		const int dy = player->getYMidPos() - getYMidPos();

		const int absdx = (dx<0) ? -dx : dx;
		const int absdy = (dy<0) ? -dy : dy;

		if( !mp_CarriedPlayer && mpActionProc == &CPlatformMoveAway::processStay )
		{
			if( absdx < CSF_DISTANCE_TO_FOLLOW_GETAWAY && absdy < CSF_DISTANCE_TO_FOLLOW_GETAWAY )
			{
				mpActionProc = &CPlatformMoveAway::processMoveAway;
			}
		}

	}

	return true;
}


void CPlatformMoveAway::processMoveAway()
{
	moveYDir(yDirection);
	moveXDir(xDirection);

	if( (m_Pos-m_Origin).GetLength() > CSF_DISTANCE_TO_GETBACK )
	{
		mpActionProc = &CPlatformMoveAway::processMoveBack;
	}
}


void CPlatformMoveAway::processMoveBack()
{
	moveDir(m_Pos-m_Origin);

	if( (m_Pos-m_Origin).GetLength() == 0 )
	{
		mpActionProc = &CPlatformMoveAway::processStay;
	}

}


void CPlatformMoveAway::processStay()
{

}


void CPlatformMoveAway::process()
{

	// Player is standing on the platform or the platform is already falling too fast
	(this->mpActionProc);

	CPlatform::process();
}

}
