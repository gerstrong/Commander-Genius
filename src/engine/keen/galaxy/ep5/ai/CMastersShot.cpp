/*
 * CMastersShot.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CMastersShot.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include <base/interface/misc.h>

/*
$2C20W #Master's shot
$2C3EW #Master's shot
$2C5CW #Master's shot
$2C7AW #Master's shot

 */


namespace galaxy {  
  
constexpr int WALK_SPEED = 25;
  
CMastersShot::CMastersShot(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y)
{
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x21DC, 0);
	
	xDirection = LEFT;
}



void CMastersShot::processWalking()
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


bool CMastersShot::isNearby(CSpriteObject &)
{
	if( !getProbability(10) )
		return false;

	return true;
}

void CMastersShot::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
        playSound(GameSound::ROBO_STUN);
		mIsDead = true;
		theObject.mIsDead = true;
	}

	/*if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}*/
}


int CMastersShot::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CMastersShot::process()
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
