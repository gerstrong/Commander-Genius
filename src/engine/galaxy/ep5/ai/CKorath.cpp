/*
 * CKorath.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CKorath.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$21DCW #Lil Ampton walk
$21FAW #Lil Ampton walk
$2218W #Lil Ampton walk
$2236W #Lil Ampton walk
$2254W #Lil Ampton turn 4
$2272W #Lil Ampton start pole slide 5
$2290W #Lil Ampton start pole slide
$22AEW #Lil Ampton pole slide 6
$22CCW #Lil Ampton stop pole slide 7
$22EAW #Lil Ampton stop pole slide
$2308W #Lil Ampton flip switch 8
$2326W #Lil Ampton flip switch 
$2344W #Lil Ampton flip switch
$2362W #Lil Ampton flip switch
$2380W #Lil Ampton flip switch
$239EW #Stunned Ampton 13
 */


namespace galaxy {  
  
enum SPARKYACTIONS
{
A_AMPTON_WALK = 0
};

const int TIME_UNTIL_MOVE = 5;
const int TIME_FOR_LOOK = 150;

const int WALK_SPEED = 25;

const int CSF_DISTANCE_TO_FOLLOW = 6<<CSF;

const int CHARGE_TIME = 250;
const int CHARGE_SPEED = 75;

const int TURN_TIME = 10;

  
CKorath::CKorath(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x21DC, A_AMPTON_WALK);
	
	xDirection = LEFT;
}



void CKorath::processWalking()
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


bool CKorath::isNearby(CSpriteObject &theObject)
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

void CKorath::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		dead = true;
		theObject.dead = true;
	}

	/*if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}*/
}


int CKorath::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CKorath::process()
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
