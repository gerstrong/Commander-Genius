/*
 * CShelly.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShelly.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$2416W #Shelly walk
$2434W #Shelly walk
$2452W #Shelly walk
$2470W #Shelly walk
$248EW #Shelly prepare to jump 4
$24ACW #Shelly jump 5
$24CAW #Shelly fall 6
$24E8W #Shelly fall
$2506W #Shelly smash 8
$2524W #Shelly smash
$2542W #Shelly smash smoke 10
$2560W #Shelly smash smoke
$257EW #Shelly smash smoke
$259CW #Shelly smash smoke
$25BAW #Shelly fragments 14
$25D8W #Shelly fragments 15 -> TODO: Need to be other actions!
 */


namespace galaxy {  
  
enum SHELLYACTIONS
{
A_SHELLY_WALK = 0,
A_SHELLY_PREPARE_JUMP = 4,
A_SHELLY_JUMP = 5,
A_SHELLY_FALL = 6,
A_SHELLY_SMASH = 8,
A_SHELLY_SMOKE = 10
};

const int WALK_SPEED = 25;

  
CShelly::CShelly(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	//mActionMap[A_AMPTON_STUNNED] = &CStunnable::processGettingStunned;
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x21DC, A_SHELLY_WALK);
	
	xDirection = LEFT;
}



void CShelly::processWalking()
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


bool CShelly::isNearby(CSpriteObject &theObject)
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

void CShelly::getTouchedBy(CSpriteObject &theObject)
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


int CShelly::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CShelly::process()
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
