/*
 * CShikadi.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShikadi.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$2D10W #Shikadi stand
$2D2EW #Shikadi stand
$2D4CW #Shikadi stand
$2D6AW #Shikadi stand
$2D88W #Shikadi walk 4
$2DA6W #Shikadi walk
$2DC4W #Shikadi walk
$2DE2W #Shikadi walk
$2E00W #Shikadi polezaps 8
$2E1EW #Shikadi polezaps
$2E3CW #Shikadi stunned 10
$2E5AW #Shikadi polezap 11 -> TODO: I think this must be another type of object
$2E78W #Shikadi polezap
 */


namespace galaxy {  
  
enum SHIKADIACTIONS
{
A_SHIKADI_STAND = 0,
A_SHIKADI_WALK = 4,
A_SHIKADI_TURN = 4,
A_SHIKADI_POLE_ZAPS = 8,
A_SHIKADI_STUNNED = 10
};

const int WALK_SPEED = 25;

  
CShikadi::CShikadi(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	//mActionMap[A_AMPTON_STUNNED] = &CStunnable::processGettingStunned;
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2D10, A_SHIKADI_STAND);
	
	xDirection = LEFT;
}



void CShikadi::processWalking()
{
  // Move normally in the direction
  if( xDirection == RIGHT )
  {
    moveRight( WALK_SPEED );
  }
  else
  {
    moveLeft( WALK_SPEED );
  }   
}


bool CShikadi::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(10) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		/*if( player->getXMidPos() < getXMidPos() )
			mKeenAlignment = LEFT;
		else
			mKeenAlignment = RIGHT;*/
	}

	return true;
}

void CShikadi::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		playSound(SOUND_ROBO_STUN);
		dead = true;
		theObject.dead = true;
	}

	/*if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}*/
}


int CShikadi::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CShikadi::process()
{
	performCollisions();
	
	performGravityMid();

	if( blockedl )
	{
	  xDirection = RIGHT;
	}
	else if(blockedr)
	{
	  xDirection = LEFT;
	}

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
