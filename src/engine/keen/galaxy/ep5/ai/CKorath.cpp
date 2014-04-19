/*
 * CKorath.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CKorath.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

/*
$30D2W #Korath walking
$30F0W #Korath walking
$310EW #Korath walking
$312CW #Korath walking
$314AW #Korath sit
$3168W #Korath stunned
 */


namespace galaxy {  
  
enum KORATHACTIONS
{
A_KORATH_WALK = 0,
A_KORATH_SIT = 4,
A_KORATH_STUNNED = 5
};

const int TIME_TO_SIT = 50;

const int WALK_SPEED = 25;
  
CKorath::CKorath(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{  
  	mActionMap[A_KORATH_WALK] = (GASOFctr) &CKorath::processWalking;
  	mActionMap[A_KORATH_SIT] = (GASOFctr) &CKorath::processSitting;
	mActionMap[A_KORATH_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;

	setupGalaxyObjectOnMap(0x30D2, A_KORATH_WALK);
	
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
  
  
  mTimer++;
  
  if( mTimer < TIME_TO_SIT)
    return;
  
  mTimer = 0;
  
  if(getProbability(120))
  {
    setAction(A_KORATH_SIT);
  }
  
}

void CKorath::processSitting()
{
  xDirection = getProbability(500) ? RIGHT : LEFT;  
  
  if(getActionStatus(A_KORATH_WALK))
  {
    setAction(A_KORATH_WALK);
  }
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
		setAction(A_KORATH_STUNNED);
	}

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
	  player->push(*this);
	}
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

	if(!dead) // If we is dead, there is no way to continue moving or turning
	{
	  if( blockedl )
	  {	    
	    xDirection = RIGHT;
	  }
	  else if(blockedr)
	  {
	    
	    xDirection = LEFT;
	  }
	}

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
