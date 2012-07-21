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
mTimer(0),
mpBolt(NULL)
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
				xDirection = LEFT;
			else
				xDirection = RIGHT;
		}

		if( mpBolt == NULL && player->getYMidPos() > getYMidPos() )
		{
			const unsigned int playXStrikeLeft = player->getXMidPos() - DIST_TO_STRIKE;
			const unsigned int playXStrikeRight = player->getXMidPos() + DIST_TO_STRIKE;


			if( playXStrikeLeft < cloudX &&
					playXStrikeRight > cloudX &&
					getProbability(90) )
			{
				setAction(A_CLOUD_STRIKING);
				mpBolt = new CThunderBolt( mp_Map, getXLeftPos() + (12<<STC), getYDownPos()-(1<<STC) );
				g_pBehaviorEngine->m_EventList.add( new EventSpawnObject( mpBolt ) );
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
	// Move normally in the direction
	if( xDirection == RIGHT )
		moveRight( MOVE_SPEED );
	else
		moveLeft( MOVE_SPEED );
}

void CThunderCloud::processStriking()
{
	if( getActionStatus(A_CLOUD_STRIKING+1) )
		setAction(A_CLOUD_STRIKING);

	if( mpBolt == NULL && mTimer % STRIKE_TIME == 0)
	{
		setAction(A_CLOUD_MOVING);
		setActionSprite();
	}

}


void CThunderCloud::process()
{
	if( mpBolt )
	{
		if(!mpBolt->exists)
			mpBolt = NULL; // This can be done because the object container will care deleting the it.
	}

	performCollisions();
	processFalling();

	if( blockedl )
		xDirection = RIGHT;
	else if( blockedr )
		xDirection = LEFT;

	(this->*mpProcessState)();

	mTimer++;

	/*if( getActionStatus(A_CLOUD_WAKING) || getActionStatus(A_CLOUD_STRIKING) )*/
	{
		processActionRoutine();
	}
}




/**
 * The thunderbold itself.
 */
const int A_LIGHTNING_STRIKE = 0;

CThunderBolt::CThunderBolt(CMap *pmap,Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, 0, x, y)
{
	setupGalaxyObjectOnMap(0x2A44, A_LIGHTNING_STRIKE);
	inhibitfall = false;
}

void CThunderBolt::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.dead )
		return;

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}

void CThunderBolt::process()
{
	performCollisions();
	if(!processActionRoutine())
			exists = false;

}


} /* namespace galaxy */
