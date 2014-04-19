/*
 * CSpirogrip.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CSpirogrip.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

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
$2932W #Spirogrip spin 9
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
    A_GRIP_MOVE_RIGHT = 18,
    A_GRIP_MOVE_UP = 19
};

const int MOVE_SPEED = 50;

const int TIME_UNTIL_SPIN = 50;
const int TIME_UNTIL_FLY = 150;
const int TIME_UNTIL_PAUSE = 50;
const int TIME_UNTIL_BACKUP = 100;

  
CSpirogrip::CSpirogrip(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
    mActionMap[A_GRIP_BACK_UP_DOWN] = (GASOFctr) &CSpirogrip::processBackup;
    mActionMap[A_GRIP_PAUSE_DOWN] = (GASOFctr) &CSpirogrip::processPause;
    mActionMap[A_GRIP_BACK_UP_LEFT] = (GASOFctr) &CSpirogrip::processBackup;
    mActionMap[A_GRIP_PAUSE_LEFT] = (GASOFctr) &CSpirogrip::processPause;
    mActionMap[A_GRIP_BACK_UP_UP] = (GASOFctr) &CSpirogrip::processBackup;
    mActionMap[A_GRIP_PAUSE_UP] = (GASOFctr) &CSpirogrip::processPause;
    mActionMap[A_GRIP_BACK_UP_RIGHT] = (GASOFctr) &CSpirogrip::processBackup;
    mActionMap[A_GRIP_PAUSE_RIGHT] = (GASOFctr) &CSpirogrip::processPause;
    mActionMap[A_GRIP_SPIN] = (GASOFctr) &CSpirogrip::processSpin;
    mActionMap[A_GRIP_MOVE_DOWN] = (GASOFctr) &CSpirogrip::processMove;
    mActionMap[A_GRIP_MOVE_LEFT] = (GASOFctr) &CSpirogrip::processMove;
    mActionMap[A_GRIP_MOVE_UP] = (GASOFctr) &CSpirogrip::processMove;
    mActionMap[A_GRIP_MOVE_RIGHT] = (GASOFctr) &CSpirogrip::processMove;    
  
    // Adapt this AI
    setupGalaxyObjectOnMap(0x2824, A_GRIP_BACK_UP_DOWN);

    xDirection = CENTER;
    yDirection = DOWN;
}

void CSpirogrip::processBackup()
{    
  mTimer++;
  if( mTimer < TIME_UNTIL_PAUSE )
      return;
  
  mTimer = 0;
    
  if(xDirection == LEFT)
      setAction(A_GRIP_PAUSE_LEFT);
  else if(xDirection == RIGHT)
      setAction(A_GRIP_PAUSE_RIGHT);

  if(yDirection == UP)
      setAction(A_GRIP_PAUSE_UP);
  else if(yDirection == DOWN)
      setAction(A_GRIP_PAUSE_DOWN);

}


void CSpirogrip::processPause()
{
  mTimer++;
  if( mTimer < TIME_UNTIL_SPIN )
      return;
  
  mTimer = 0;
    
  setAction(A_GRIP_SPIN);

}

void CSpirogrip::processSpin()
{
  mTimer++;
  if( mTimer < TIME_UNTIL_FLY )
      return;
  
  mTimer = 0;

  playSound(SOUND_SPIROFLY);
  
  // Look at the Player coords and define a direction
  xDirection = yDirection = CENTER;
  if(getProbability(500))
      xDirection = mKeenAlignmentX;
  else
      yDirection = mKeenAlignmentY;
  
  if(xDirection == LEFT)
      setAction(A_GRIP_MOVE_LEFT);
  else if(xDirection == RIGHT)
      setAction(A_GRIP_MOVE_RIGHT);

  if(yDirection == UP)
      setAction(A_GRIP_MOVE_UP);
  else if(yDirection == DOWN)
      setAction(A_GRIP_MOVE_DOWN);
  
}



void CSpirogrip::processMove()
{
  // Move normally in the direction
  moveXDir( xDirection*MOVE_SPEED );
  moveYDir( yDirection*MOVE_SPEED );
  
  if( blockedl )
  {
    xDirection = RIGHT;
    yDirection = CENTER;
    setAction(A_GRIP_BACK_UP_RIGHT);    
  }
  else if( blockedr )
  {
    xDirection = LEFT;
    yDirection = CENTER;
    setAction(A_GRIP_BACK_UP_LEFT);
  }
	
  if( blockedu )
  {
    xDirection = CENTER;
    yDirection = UP;
    setAction(A_GRIP_BACK_UP_UP);
  }
  else if( blockedd )
  {
    xDirection = CENTER;
    yDirection = DOWN;
    setAction(A_GRIP_BACK_UP_DOWN);
  }
  
  mTimer++;
  if( mTimer < TIME_UNTIL_BACKUP )
      return;
  
  mTimer = 0;
  
  if(getProbability(500))
  {            
      if(xDirection == LEFT)
	  setAction(A_GRIP_BACK_UP_LEFT);
      else if(xDirection == RIGHT)
	  setAction(A_GRIP_BACK_UP_RIGHT);
      
      if(yDirection == UP)
	  setAction(A_GRIP_BACK_UP_UP);
      else if(yDirection == DOWN)
	  setAction(A_GRIP_BACK_UP_DOWN);
  }
      
}


bool CSpirogrip::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{	    	    
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignmentX = LEFT;
		else
			mKeenAlignmentX = RIGHT;

		if( player->getYMidPos() < getYMidPos() )
			mKeenAlignmentY = UP;
		else
			mKeenAlignmentY = DOWN;
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
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


void CSpirogrip::process()
{
	performCollisions();		

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
