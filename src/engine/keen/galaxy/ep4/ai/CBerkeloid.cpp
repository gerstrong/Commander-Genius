/*
 * CBerkeloid.cpp
 *
 *  Created on: 16.07.2012
 *      Author: gerstong
 */

#include "CBerkeloid.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include <base/utils/misc.h>

namespace galaxy {

const int A_BERKELOID_MOVING = 0;
const int A_BERKELOID_THROW = 4;

const int BERKELOID_TIME = 5;
	

CBerkeloid::CBerkeloid(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0),
mTimer(0),
mpProcessState(nullptr)
{
	mActionMap[A_BERKELOID_MOVING] = &CBerkeloid::processMoving;
	mActionMap[A_BERKELOID_THROW] = &CBerkeloid::processThrowing;

	setupGalaxyObjectOnMap(0x2AF8, A_BERKELOID_MOVING);
	inhibitfall = true;

	performCollisions();
	processActionRoutine();
	
	if(blockedl || blockedr || blockedd)
	{
	  moveUp(8<<STC);
	}
}

void CBerkeloid::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mpProcessState = mActionMap[ActionNumber];
	else
		setActionForce(0); // This might happen, when the action-map is incomplete
}


void CBerkeloid::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.dead )
		return;

	if( CBullet *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		bullet->dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}

bool CBerkeloid::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(30) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			xDirection = LEFT;
		else
			xDirection = RIGHT;
	}

	return true;
}


void CBerkeloid::processMoving()
{
  if(blockedd)
    moveUp(10);
  
	if( mTimer < BERKELOID_TIME )
	{
		mTimer++;
		return;
	}
	else
	{
		mTimer = 0;
	}

	// Chance to throw a flame
	if( getProbability(20) )
	{
		setAction( A_BERKELOID_THROW );
		playSound( SOUND_BERKELOID_WINDUP );
		CBerkFlame *flame = new CBerkFlame( getMapPtr(), getXMidPos(), getYUpPos(), xDirection );
		gEventManager.add( new EventSpawnObject( flame ) );
		return;
	}

}

void CBerkeloid::processThrowing()
{
	if( getActionStatus(A_BERKELOID_MOVING) )
		setAction(A_BERKELOID_MOVING);
}


void CBerkeloid::process()
{

	//processFalling();

	performCollisions();

	(this->*mpProcessState)();

	if( blockedl )
		xDirection = RIGHT;
	else if(blockedr)
		xDirection = LEFT;

	if(!processActionRoutine())
			exists = false;

}



 ////////////////////////////////// End of Berkeloid //////////////////////////////////

/*
 *
 * Flame Part of the Berkeloid
 *
 */
const int A_FLAME_THROWN = 0;
const int A_FLAME_LANDED = 2;

const int FLAME_INERTIAX = 150;

CBerkFlame::CBerkFlame(CMap *pmap, Uint32 x, Uint32 y, const int xDir) :
CGalaxySpriteObject(pmap, 0, x, y, 0),
mpProcessState(NULL)
{
	mActionMap[A_FLAME_THROWN] = &CBerkFlame::processThrown;
	mActionMap[A_FLAME_LANDED] = &CBerkFlame::processLanded;

	xDirection = xDir;
	xinertia = FLAME_INERTIAX;

	setupGalaxyObjectOnMap(0x2CD8, A_FLAME_THROWN);
}


void CBerkFlame::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mpProcessState = mActionMap[ActionNumber];
	else
		setActionForce(0); // This might happen, when the action-map is incomplete
}


void CBerkFlame::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.dead )
		return;

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


void CBerkFlame::processThrown()
{
	if(blockedd)
		setAction(A_FLAME_LANDED);

	// Move normally in the direction
	if( xDirection == RIGHT )
		moveRight( xinertia );
	else
		moveLeft( xinertia );

	xinertia--;
}

void CBerkFlame::processLanded()
{

}


void CBerkFlame::process()
{

	processFalling();

	performCollisions();

	(this->*mpProcessState)();

	if(!processActionRoutine())
			exists = false;

}


} /* namespace galaxy */
