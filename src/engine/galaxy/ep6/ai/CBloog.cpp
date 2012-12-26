/*
 * CBloog.cpp
 *
 *  Created on: 26 Dez 2012
 *      Author: Gerstrong
 */


#include "CBloog.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include "misc.h"

namespace galaxy {

CBloog::CBloog(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y)
{
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2012, 0);
}



void CBloog::processMoving()
{
	// Move normally in the direction
	if( xDirection == RIGHT )
	{
		moveRight( m_Action.velX<<1 );
	}
	else
	{
		moveLeft( m_Action.velX<<1 );
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
		//setAction(A_SPARKY_STUNNED);
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
	
	performGravityLow();			

	if( blockedl )
		xDirection = RIGHT;
	else if(blockedr)
		xDirection = LEFT;

	if(!processActionRoutine())
	    exists = false;
	
//	(this->*mp_processState)();
	processMoving();

}

}
