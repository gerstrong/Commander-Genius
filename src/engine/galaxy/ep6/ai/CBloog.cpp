/*
 * CBloog.cpp
 *
 *  Created on: 26 Dez 2012
 *      Author: Gerstrong
 */


#include "CBloog.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"


namespace galaxy {
  
enum BLOOGACTIONS
{
A_BLOOG_WALK = 0,	/* Ordinary slug_move action */
A_BLOOG_STUNNED = 4
};

const int WALK_SPEED = 30;


CBloog::CBloog(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	mActionMap[A_BLOOG_WALK] = (void (CStunnable::*)()) &CBloog::processWalking;
	mActionMap[A_BLOOG_STUNNED] = &CStunnable::processGettingStunned;
	
	setupGalaxyObjectOnMap(0x1EE6, A_BLOOG_WALK);
	
	xDirection = LEFT;
}



void CBloog::processWalking()
{
	// Move normally in the direction
	if( xDirection == RIGHT )
	{
		//moveRight( m_Action.velX );
		moveRight( WALK_SPEED );
	}
	else
	{
		
		//moveLeft( m_Action.velX );
		moveLeft( WALK_SPEED );
	}
}


void CBloog::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction(A_BLOOG_STUNNED);
		dead = true;
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


int CBloog::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CBloog::process()
{
	performCollisions();
	
	performGravityHigh();

	if( blockedl )
		xDirection = RIGHT;
	else if(blockedr)
		xDirection = LEFT;

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();

}

}
