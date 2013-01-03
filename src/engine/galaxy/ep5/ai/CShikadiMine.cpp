/*
 * CShikadiMine.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShikadiMine.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$2608W #Shikadi mine sit 0
$2626W #Shikadi mine move 1
$2644W #Shikadi mine change direction 2
$2662W #Shikadi mine detonate 3
$2680W #Shikadi mine detonate
$269EW #Shikadi mine detonate
$26BCW #Shikadi mine detonate
$26DAW #Shikadi mine detonate
$26F8W #Shikadi mine detonate
$2716W #Shikadi mine fragments -> TODO: This is another object!
 */


namespace galaxy {  
  
enum MINEACTIONS
{
A_MINE_SIT = 0,
A_MINE_MOVE = 1,
A_MINE_CHANGE_DIR = 2,
A_MINE_DETONATE = 3
};

const int TIME_UNTIL_MOVE = 5;
const int TIME_FOR_LOOK = 150;

const int WALK_SPEED = 25;

const int CSF_DISTANCE_TO_FOLLOW = 6<<CSF;

const int CHARGE_TIME = 250;
const int CHARGE_SPEED = 75;

const int TURN_TIME = 10;

  
CShikadiMine::CShikadiMine(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	//mActionMap[A_AMPTON_STUNNED] = &CStunnable::processGettingStunned;
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2608, A_MINE_SIT);
	
	xDirection = LEFT;
}



void CShikadiMine::processWalking()
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


bool CShikadiMine::isNearby(CSpriteObject &theObject)
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

void CShikadiMine::getTouchedBy(CSpriteObject &theObject)
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


int CShikadiMine::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CShikadiMine::process()
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
