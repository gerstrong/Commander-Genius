/*
 * CShockshound.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShockshound.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$2E96W #Shocksund sit
$2EB4W #Shocksund sit
$2ED2W #Shocksund bark 2
$2EF0W #Shocksund bark
$2F0EW #Shocksund walk 4
$2F2CW #Shocksund walk
$2F4AW #Shocksund walk
$2F68W #Shocksund walk
$2F86W #Shocksund jump 8
$2FA4W #Shocksund shot 9 -> TODO: Another Object?
$2FC2W #Shocksund shot
$2FE0W #Shocksund shot smash 11
$2FFEW #Shocksund shot smash
$301CW #Shocksund stunned 13
 */


namespace galaxy {  
  
enum HOUNDACTIONS
{
A_HOUND_SIT = 0,
A_HOUND_BARK = 2,
A_HOUND_WALK = 4,
A_HOUND_JUMP = 8,
A_HOUND_STUNNED = 13
};

const int TIME_UNTIL_MOVE = 5;
const int TIME_FOR_LOOK = 150;

const int WALK_SPEED = 25;

const int CSF_DISTANCE_TO_FOLLOW = 6<<CSF;

const int CHARGE_TIME = 250;
const int CHARGE_SPEED = 75;

const int TURN_TIME = 10;

  
CShockshound::CShockshound(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	//mActionMap[A_AMPTON_STUNNED] = &CStunnable::processGettingStunned;
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2E96, A_HOUND_SIT);
	
	xDirection = LEFT;
}



void CShockshound::processWalking()
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


bool CShockshound::isNearby(CSpriteObject &theObject)
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

void CShockshound::getTouchedBy(CSpriteObject &theObject)
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


int CShockshound::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CShockshound::process()
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
