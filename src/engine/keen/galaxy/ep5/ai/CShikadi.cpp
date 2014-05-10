/*
 * CShikadi.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShikadi.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CEnemyShot.h"
#include <base/utils/misc.h>

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
$2E5AW #Shikadi polezap 11
$2E78W #Shikadi polezap
 */


namespace galaxy {  
  
enum SHIKADIACTIONS
{
A_SHIKADI_STAND = 0,
A_SHIKADI_WALK = 4,
A_SHIKADI_POLE_ZAPS = 8,
A_SHIKADI_STUNNED = 10
};

const int WALK_SPEED = 25;

const int TIME_UNTIL_STAND = 150;

const int TIME_TO_ZAP = 30;
  
CShikadi::CShikadi(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mHealth(4),
mTimer(0)
{
    mActionMap[A_SHIKADI_STAND] = (GASOFctr) &CShikadi::processStanding;
    mActionMap[A_SHIKADI_WALK] = (GASOFctr) &CShikadi::processWalking;
    mActionMap[A_SHIKADI_POLE_ZAPS] = (GASOFctr) &CShikadi::processPoleZaps;
    mActionMap[A_SHIKADI_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
  
    // Adapt this AI
    setupGalaxyObjectOnMap(0x2D10, A_SHIKADI_STAND);
    
    xDirection = LEFT;
}

void CShikadi::processStanding()
{
    if(getActionStatus(A_SHIKADI_WALK))
    {
	setAction(A_SHIKADI_WALK);
    }
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
  
  if( blockedl )
  {
      xDirection = RIGHT;
  }
  else if(blockedr)
  {
      xDirection = LEFT;
  }
  
  mTimer++;
  if( mTimer < TIME_UNTIL_STAND )  
      return;
  
  mTimer = 0;
  
  int newX = (xDirection == LEFT) ? getXLeftPos()+(4<<STC) : getXRightPos()-(4<<STC);
  
  if(hitdetectWithTilePropertyVert(1, newX, getYUpPos(), getYDownPos(), (1<<STC) ))
  {
      if(getProbability(500))
      {
          newX = (newX>>CSF)<<CSF;
          // Spawn a Enemyshot in form electrostatic  charge which goes upwards
          spawnObj( new CEnemyShot(mp_Map, 0,
                                                                  newX, getYUpPos(),
                                                                  0x2E5A, 0, UP,  150, mSprVar) );
          setAction(A_SHIKADI_POLE_ZAPS);
          playSound(SOUND_POLEZAP);
          return;
      }
  }

  if(getProbability(300))
  {
    setAction(A_SHIKADI_STAND);
  }

}

void CShikadi::processPoleZaps()
{    
    if(getActionStatus(A_SHIKADI_WALK))
    {
	setAction(A_SHIKADI_WALK);
    }
}


bool CShikadi::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(10) )
		return false;

	return true;
}

void CShikadi::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

    CStunnable::getTouchedBy(theObject);

    // Was it a bullet? Than make loose health.
    if( dynamic_cast<CBullet*>(&theObject) )
    {
        mHealth--;
        theObject.dead = true;

        if(mHealth == 0)
        {
            setAction(A_SHIKADI_STUNNED);
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


int CShikadi::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CShikadi::process()
{
	performCollisions();
	
	performGravityMid();

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
