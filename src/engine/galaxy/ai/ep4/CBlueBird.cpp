/*
 * CBlueBird.cpp
 *
 *  Created on: 11.07.2012
 *      Author: gerstong
 */

#include "CBlueBird.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "engine/galaxy/ai/CBullet.h"
#include "misc.h"

namespace galaxy {

const int A_EAGLE_HATCHED = 0;
const int A_EAGLE_WALKING = 1;
const int A_EAGLE_FLYING = 5;
const int A_EAGLE_STUNNED = 10;

const int WALK_SPEED = 34;
const int FLY_SPEED = 34;
const int CHANCETOFLY = 50;
const int CHANCETOSTAND = 50;

CBlueBird::CBlueBird(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
mTimer(0)
{
	mActionMap[A_EAGLE_HATCHED] = &CBlueBird::processHatched;
	mActionMap[A_EAGLE_WALKING] = &CBlueBird::processWalking;
	mActionMap[A_EAGLE_WALKING+1] = &CBlueBird::processWalking;
	mActionMap[A_EAGLE_FLYING] =  &CBlueBird::processFlying;
	mActionMap[A_EAGLE_STUNNED] = &CBlueBird::processStunned;

	setupGalaxyObjectOnMap( 0x21B6, A_EAGLE_HATCHED );
	xDirection = LEFT;

	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	performCollisions();
	processMove( 0, rSprite.m_bboxY1-rSprite.m_bboxY2 );
	processActionRoutine();

}

void CBlueBird::setActionForce(const size_t ActionNumber)
{
	CGalaxySpriteObject::setActionForce(ActionNumber);

	if( mActionMap.find(ActionNumber) != mActionMap.end() )
		mpProcessState = mActionMap[ActionNumber];
	else
		setActionForce(0); // This might happen, when the action-map is incomplete
}

bool CBlueBird::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(80) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			xDirection = LEFT;
		else
			xDirection = RIGHT;

		if( player->getYMidPos() < getYMidPos() )
			yDirection = UP;
		else
			yDirection = DOWN;
	}

	return true;
}

void CBlueBird::processHatched()
{
	if( getActionStatus(A_EAGLE_WALKING) )
		setAction(A_EAGLE_WALKING);
}

void CBlueBird::processWalking()
{
	if( mTimer % CHANCETOFLY == 0 )
	{
		// Chance to poo
		if( getProbability(30) )
		{
			setAction(A_EAGLE_FLYING);
			inhibitfall = true;
		}
	}

	mTimer++;

	// Move normally in the direction
	if( xDirection == RIGHT )
		moveRight( WALK_SPEED );
	else
		moveLeft( WALK_SPEED );
}

void CBlueBird::processFlying()
{
	if(inhibitfall)
	{
		if( mTimer % CHANCETOFLY == 0 )
		{
			// Chance to land
			if( getProbability(20) )
				inhibitfall = false;
		}

		mTimer++;

		// Move normally in the direction
		if( xDirection == RIGHT )
			moveRight( FLY_SPEED );
		else
			moveLeft( FLY_SPEED );

		if( yDirection == UP )
			moveUp( FLY_SPEED );
		else
			moveDown( FLY_SPEED );
	}
	else
	{
		if(blockedd)
			setAction(A_EAGLE_WALKING);
	}
}

void CBlueBird::processStunned()
{
	if( getActionStatus(A_EAGLE_WALKING) )
		setAction(A_EAGLE_WALKING);
}

void CBlueBird::getTouchedBy(CSpriteObject &theObject)
{
	if( theObject.dead )
		return;

	if( CBullet *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		bullet->dead = true;
		inhibitfall = false;
		setAction(A_EAGLE_STUNNED);
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}

void CBlueBird::process()
{
	performCollisions();
	processFalling();

	if( blockedl )
		xDirection = RIGHT;
	else if( blockedr )
		xDirection = LEFT;

	if( blockedu )
		yDirection = DOWN;
	else if( blockedu )
		yDirection = UP;


	(this->*mpProcessState)();

	if(!processActionRoutine())
			exists = false;
}

} /* namespace galaxy */
