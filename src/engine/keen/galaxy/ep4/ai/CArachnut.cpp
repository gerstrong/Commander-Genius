/*
 * CArachnut.cpp
 *
 *  Created on: 17.07.2012
 *      Author: gerstong
 */

#include "CArachnut.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include <base/utils/misc.h>


namespace galaxy
{

const int A_ARACHNUT_WALK = 0;
const int A_ARACHNUT_STUNNED = 4;

const int WALK_SPEED = 45;

CArachnut::CArachnut(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0)
{
	setupGalaxyObjectOnMap( 0x2378, A_ARACHNUT_WALK );
	xDirection = LEFT;

    GsSprite &rSprite = gGraphics.getSprite(mSprVar,sprite);
	performCollisions();
	processMove( 0, rSprite.m_bboxY1-rSprite.m_bboxY2 );
	processActionRoutine();
}

bool CArachnut::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(10) )
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

void CArachnut::getTouchedBy(CSpriteObject &theObject)
{
	if( theObject.dead || getActionStatus(A_ARACHNUT_STUNNED) )
		return;

	if( CBullet *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		setAction(A_ARACHNUT_STUNNED);
	}
	
	if(getActionNumber(A_ARACHNUT_STUNNED))
	    return;

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


int CArachnut::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}



void CArachnut::process()
{
	performCollisions();
	performGravityLow();

	if( blockedl )
		xDirection = RIGHT;
	else if( blockedr )
		xDirection = LEFT;

	if( getActionStatus(A_ARACHNUT_WALK) )
		setAction(A_ARACHNUT_WALK);

	if( getActionNumber(A_ARACHNUT_WALK) )
	{
		// Move normally in the direction
		if( xDirection == RIGHT )
			moveRight( WALK_SPEED );
		else
			moveLeft( WALK_SPEED );
	}


	if(!processActionRoutine())
			exists = false;
}

} /* namespace galaxy */
