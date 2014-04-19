/*
 * CShockshound.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShockshound.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CEnemyShot.h"
#include <base/utils/misc.h>

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
A_HOUND_SHOOT = 9,
A_HOUND_STUNNED = 13
};

const int WALK_SPEED = 25;

const int TIME_TO_BARK = 64;
const int TIME_TO_SIT = 32;
const int TIME_WALKING = 50;

  
CShockshound::CShockshound(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mHealth(2),
mTimer(0)
{
	
	mActionMap[A_HOUND_SIT] = (GASOFctr) &CShockshound::processSit;
	mActionMap[A_HOUND_BARK] = (GASOFctr) &CShockshound::processBark;
	mActionMap[A_HOUND_WALK] = (GASOFctr) &CShockshound::processWalking;
	mActionMap[A_HOUND_JUMP] = (GASOFctr) &CShockshound::processJump;
	mActionMap[A_HOUND_SHOOT] = (GASOFctr) &CShockshound::processBark;
	mActionMap[A_HOUND_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
	
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2E96, A_HOUND_SIT);
	
	xDirection = LEFT;
	yDirection = CENTER;
}



void CShockshound::processSit()
{
    mTimer++;
    if( mTimer < TIME_TO_SIT )
      return;
  
    mTimer = 0;
    
    setAction(A_HOUND_BARK);  
    playSound(SOUND_SHOCKSUNDBARK);
}


void CShockshound::processBark()
{
    mTimer++;
    if( mTimer < TIME_TO_BARK )
      return;
  
    mTimer = 0;
    
    xDirection = mKeenAlignmentX;
    
    if(getProbability(200))
    {
	setAction(A_HOUND_SIT);    
    }

    if(mKeenAlignmentY != CENTER)
    {
	yinertia = -120;
	setAction(A_HOUND_JUMP);
    }    
    
    if(getProbability(500))
    {	    
	// Spawn a Enemyshot in form electrostatic
	const int newX = (xDirection == LEFT) ? getXLeftPos()+(4<<STC) : getXRightPos()-(4<<STC);
	spawnObj( new CEnemyShot(mp_Map, 0, 
							newX, getYUpPos()-(8<<STC),
                            0x2FC2, xDirection, CENTER,  150, mSprVar) );
	
	// SD_PlaySound(SOUND_BARKSHOTDIE); This must be used in the Enemyshot class, but can't because it's too general
    }

}


void CShockshound::processWalking()
{
  // Move normally in the direction
  moveXDir(xDirection*WALK_SPEED);  
  
  mTimer++;
  if( mTimer < TIME_WALKING )
    return;
  
  mTimer = 0;
    
  if(getProbability(200))
  {
    setAction(A_HOUND_SIT);    
  }    
}

void CShockshound::processJump()
{
  // Move normally in the direction
  moveXDir(xDirection*WALK_SPEED);  

  if( yinertia >= 0 && blockedd)
  {
      setAction(A_HOUND_WALK);
      yinertia = 0;
  }
}



bool CShockshound::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{	    	    
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignmentX = LEFT;
		else
			mKeenAlignmentX = RIGHT;
		
		mKeenAlignmentY = CENTER;
		if( player->getYDownPos()-(1<<STC) < getYUpPos() )
		    mKeenAlignmentY = UP;
		else if( player->getYDownPos()+(1<<STC) > getYDownPos() )
		    mKeenAlignmentY = DOWN;
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
	    mHealth--;
	    theObject.dead = true;
	    
	    // TODO: Flash effect must be added here!
	    if(mHealth == 0)
	    {
	      setAction(A_HOUND_STUNNED);
	      dead = true;
	    }		
	    else
	    {
	      blink(10);
	    }
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
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
