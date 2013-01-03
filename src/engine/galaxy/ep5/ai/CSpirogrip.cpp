/*
 * CSpirogrip.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CSpirogrip.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$2824W #Spirogrip back up down 0
$2842W #Spriogrip pause down 1
$2860W #Spirogrip back up left 2
$287EW #Spirogrip pause left 3
$289CW #Spirogrip back up up 4
$28BAW #Spirogrip pause up 5
$28D8W #Spirogrip back up right 6
$28F6W #Spirogrip puase right 7
$2914W #Spirogrip spin 8
$2932W #Spirogrip spin
$2950W #Spirogrip spin
$296EW #Spirogrip spin
$298CW #Spirogrip spin
$29AAW #Spirogrip spin
$29C8W #Spirogrip spin
$29E6W #Spirogrip spin
$2A04W #Spirogrip move down 16
$2A22W #Spirogrip move left 17
$2A40W #Spirogrip move up 18
$2A5EW #Spirogrip move right 19
 */


namespace galaxy {  
  
enum GRIPACTIONS
{
    A_GRIP_BACK_UP_DOWN = 0,
    A_GRIP_PAUSE_DOWN = 1,
    A_GRIP_BACK_UP_LEFT = 2,
    A_GRIP_PAUSE_LEFT = 3,
    A_GRIP_BACK_UP_UP = 4,
    A_GRIP_PAUSE_UP = 5,
    A_GRIP_BACK_UP_RIGHT = 6,
    A_GRIP_PAUSE_RIGHT = 7,
    A_GRIP_SPIN = 8,
    A_GRIP_MOVE_DOWN = 16,
    A_GRIP_MOVE_LEFT = 17,
    A_GRIP_MOVE_UP = 18,
    A_GRIP_MOVE_RIGHT = 19
};

const int MOVE_SPEED = 25;

  
CSpirogrip::CSpirogrip(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
    // TODO: -> Actionmap
	mActionMap[A_GRIP_BACK_UP_DOWN] = &CStunnable::processGettingStunned;
	
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2824, A_GRIP_BACK_UP_DOWN);
	
	xDirection = LEFT;
}



void CSpirogrip::processWalking()
{
  // Move normally in the direction
  if( xDirection == RIGHT )
  {
    moveRight( MOVE_SPEED );
  }
  else
  {
    moveLeft( MOVE_SPEED );
  }   
}


bool CSpirogrip::isNearby(CSpriteObject &theObject)
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

void CSpirogrip::getTouchedBy(CSpriteObject &theObject)
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


int CSpirogrip::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CSpirogrip::process()
{
	performCollisions();
	
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
