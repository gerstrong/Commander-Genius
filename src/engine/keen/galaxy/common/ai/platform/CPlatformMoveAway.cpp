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
const int MOVE_SPEED_ACC = 1;

// Speed at what the Platform might return to the original position
const int MOVE_NORMAL_SPEED = 25;

// Top Speed at which the plat moves away
const int MOVE_MAX_SPEED = 100;

// The distance to get the platform moved
const int CSF_DISTANCE_TO_FOLLOW_GETAWAY = 2<<CSF;

// The distance to get the platform moved
const int CSF_DISTANCE_TO_GETBACK = 7<<CSF;


namespace galaxy {

CPlatformMoveAway::CPlatformMoveAway(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
        const direction_t vertdir, const direction_t hordir, const int actionOffset, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
CPlatform(pmap, foeID, x, y),
m_Origin(m_Pos)
{
	xDirection = hordir;
	yDirection = vertdir;
	solid = false;
	m_ActionBaseOffset = actionOffset;
	setActionForce(A_PLATFORM_MOVE);
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

        if( mCarriedPlayerVec.empty() && mpActionProc == &CPlatformMoveAway::processStay )
		{
			if( absdx < CSF_DISTANCE_TO_FOLLOW_GETAWAY && absdy < CSF_DISTANCE_TO_FOLLOW_GETAWAY )
			{
				mpActionProc = &CPlatformMoveAway::processMoveAway;
				mSpeed = MOVE_MAX_SPEED;
			}
		}

	}

	return true;
}


void CPlatformMoveAway::processMoveAway()
{
	const int x2 = m_Pos.x;
	const int y2 = m_Pos.y;
	const int x1 = m_Origin.x;
	const int y1 = m_Origin.y;

    Vector2D<int> dist(x2-x1, y2-y1);


	movePlatY(yDirection*mSpeed);
	movePlatX(xDirection*mSpeed);

	if( mSpeed > MOVE_NORMAL_SPEED )
		mSpeed -= MOVE_SPEED_ACC;

	const int absdx = (dist.x<0) ? -dist.x : dist.x;
	const int absdy = (dist.y<0) ? -dist.y : dist.y;


	if( absdx > CSF_DISTANCE_TO_GETBACK ||
		absdy > CSF_DISTANCE_TO_GETBACK)
	{
		mpActionProc = &CPlatformMoveAway::processMoveBack;
		mSpeed = MOVE_NORMAL_SPEED;
	}
}


void CPlatformMoveAway::processMoveBack()
{
	const int x2 = m_Pos.x;
	const int y2 = m_Pos.y;
	const int x1 = m_Origin.x;
	const int y1 = m_Origin.y;

    Vector2D<int> dist(x2-x1, y2-y1);

	movePlatY(-yDirection*mSpeed);
	movePlatX(-xDirection*mSpeed);

	const int absdx = (dist.x<0) ? -dist.x : dist.x;
	const int absdy = (dist.y<0) ? -dist.y : dist.y;

	if( absdx < MOVE_MAX_SPEED &&
	    absdy < MOVE_MAX_SPEED )
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
	(this->*mpActionProc)();

	CPlatform::process();
	
	if(getActionStatus(A_PLATFORM_DROP))
	{
	    setAction(A_PLATFORM_MOVE);
	}
}

}
