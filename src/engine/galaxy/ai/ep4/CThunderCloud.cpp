/*
 * CThunderCloud.cpp
 *
 *  Created on: 12.07.2012
 *      Author: gerstong
 */

#include "CThunderCloud.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "engine/galaxy/ai/CBullet.h"
#include "misc.h"

namespace galaxy {

const int A_CLOUD_ASLEEP = 0;
const int A_CLOUD_WAKING = 1;
const int A_CLOUD_MOVING = 2;
const int A_CLOUD_STRIKING = 5;

const unsigned int DIST_TO_STRIKE = 1<<CSF;
const unsigned int DIST_TO_AWAKE = 7<<CSF;

const int MOVE_SPEED = 34;
const int STRIKE_TIME = 80;

CThunderCloud::CThunderCloud(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
mTimer(0)
{
	mActionMap[A_CLOUD_ASLEEP] = &CThunderCloud::processAsleep;
	mActionMap[A_CLOUD_WAKING] = &CThunderCloud::processWaking;
	mActionMap[A_CLOUD_MOVING] = &CThunderCloud::processMoving;
	mActionMap[A_CLOUD_STRIKING] = &CThunderCloud::processStriking;

	setupGalaxyObjectOnMap(0x28A0, A_CLOUD_ASLEEP);
	inhibitfall = true;
}



void CThunderCloud::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mpProcessState = mActionMap[ActionNumber];
	else
		setActionForce(0); // This might happen, when the action-map is incomplete
}

bool CThunderCloud::isNearby(CSpriteObject &theObject)
{

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		const unsigned int cloudX = getXMidPos();
		const unsigned int playXLeft = player->getXMidPos() - DIST_TO_AWAKE;
		const unsigned int playXRight = player->getXMidPos() + DIST_TO_AWAKE;

		if( playXLeft < cloudX &&
				playXRight > cloudX )
		{
			if( getActionStatus(A_CLOUD_ASLEEP) && getProbability(200) )
				setAction(A_CLOUD_WAKING);
		}
		else
		{
			if( !getActionStatus(A_CLOUD_ASLEEP) )
			{
				setAction(A_CLOUD_ASLEEP);
				setActionSprite();
				return true;
			}

		}


		if( getProbability(80) && getActionStatus(A_CLOUD_MOVING) )
		{
			if( player->getXMidPos() < getXMidPos() )
				m_hDir = LEFT;
			else
				m_hDir = RIGHT;
		}

		if( player->getYMidPos() > getYMidPos() )
		{
			const unsigned int playXStrikeLeft = player->getXMidPos() - DIST_TO_STRIKE;
			const unsigned int playXStrikeRight = player->getXMidPos() + DIST_TO_STRIKE;


			if( playXStrikeLeft < cloudX &&
					playXStrikeRight > cloudX &&
					getProbability(90) )
			{
				setAction(A_CLOUD_STRIKING);
			}


		}

	}

	return true;
}

void CThunderCloud::processAsleep()
{
}

void CThunderCloud::processWaking()
{
	if( getActionStatus(A_CLOUD_MOVING) )
		setAction(A_CLOUD_MOVING);
}

void CThunderCloud::processMoving()
{
	if( getActionStatus(A_CLOUD_STRIKING) )
		setAction(A_CLOUD_STRIKING);

	// Move normally in the direction
	if( m_hDir == RIGHT )
		moveRight( MOVE_SPEED );
	else
		moveLeft( MOVE_SPEED );
}

void CThunderCloud::processStriking()
{
	if( getActionStatus(A_CLOUD_STRIKING+1) )
		setAction(A_CLOUD_STRIKING);

	if( mTimer % STRIKE_TIME == 0)
	{
		setAction(A_CLOUD_MOVING);
		setActionSprite();
	}

}


void CThunderCloud::process()
{
	performCollisions();
	processFalling();

	if( blockedl )
		m_hDir = RIGHT;
	else if( blockedr )
		m_hDir = LEFT;

	(this->*mpProcessState)();

	mTimer++;

	if( getActionStatus(A_CLOUD_WAKING) || getActionStatus(A_CLOUD_STRIKING) )
		processActionRoutine();
}


} /* namespace galaxy */
