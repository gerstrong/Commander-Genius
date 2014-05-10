/*
 * CRoboRed.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CRoboRed.h"
#include "CRedShot.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

/*
$2734W #Robo Red move
$2752W #Robo Red pause before shooting
$2770W #Robo Red shoot
$278EW #Robo Red shoot (Make shot)
*/

namespace galaxy {  
  
enum ROBOREDACTIONS
{
A_RED_MOVE = 0,
A_RED_PAUSE = 1,
A_RED_SHOOT = 3
};

const int TIME_UNTIL_SHOOT = 80;
const int TIME_UNTIL_LOOK = 200;

const int TIME_SHOOTING = 200;


const int CSF_DISTANCE_TO_SHOOT = 8<<CSF;

  
CRoboRed::CRoboRed(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0),
mLookTimer(0),
swapYDir(false),
mKeenNearby(false)
{
	mActionMap[A_RED_MOVE] = (GASOFctr) &CRoboRed::processMoving;
	mActionMap[A_RED_PAUSE] = (GASOFctr) &CRoboRed::processPauseBeforeShoot;
	mActionMap[A_RED_SHOOT] = (GASOFctr) &CRoboRed::processShoot;
  

    auto diff = g_pBehaviorEngine->mDifficulty;

    moveHorizSpeed = 15;

    if(diff > NINJA)
    {
        moveHorizSpeed = 30;
    }
    if(diff > EXPERT)
    {
        moveHorizSpeed = 25;
    }
    if(diff > HARD)
    {
        moveHorizSpeed = 20;
    }


	// Adapt this AI
	setupGalaxyObjectOnMap(0x2734, A_RED_MOVE);
	
	xDirection = LEFT;
}



void CRoboRed::processMoving()
{
  // Move normally in the direction
  if( xDirection == RIGHT )
  {
    moveRight( moveHorizSpeed );
  }
  else
  {
    moveLeft( moveHorizSpeed );
  }
  
  if(getProbability(60) && mKeenNearby)
  {
    setAction(A_RED_PAUSE);
  }
}


void CRoboRed::processPauseBeforeShoot()
{
  // just wait 
  mTimer++;
  if(mTimer < TIME_UNTIL_SHOOT)
    return;
  
  mTimer = 0;
  
  if(mKeenNearby)
    setAction(A_RED_SHOOT);  
  else
    setAction(A_RED_MOVE);
}


void CRoboRed::processShoot()
{
  // Shoot many times.  
  if(mTimer%16 == 0)
  {
    playSound(SOUND_ROBORED_SHOOT);
    
    direction_t newXDir = xDirection<0 ? LEFT : RIGHT;
    direction_t newYDir = swapYDir ? UP : DOWN;    
    swapYDir = !swapYDir;
    int newX = 	xDirection == RIGHT ? getXRightPos() : getXLeftPos();
    int newY = 	getYPosition() + 0x300;
    spawnObj( new CRedShot( getMapPtr(), 
							     0, 
							     newX, newY,
							     newXDir, newYDir ) );
  }
				
  mTimer++;
  if(mTimer < TIME_SHOOTING)
    return;
  
  mTimer = 0;
  
  setAction(A_RED_PAUSE);
}



bool CRoboRed::isNearby(CSpriteObject &theObject)
{
    if( dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        mLookTimer++;
        if(mLookTimer < TIME_UNTIL_LOOK)
            return true;

        mLookTimer = 0;

        mKeenNearby = false;

        const int objX = theObject.getXMidPos();
        const int roboX = getXMidPos();
        const int dx = objX - roboX;

        if( theObject.getYDownPos() > getYUpPos() && theObject.getYUpPos() < getYDownPos() )
        {
            if(getActionNumber(A_RED_PAUSE))
            {
                if( theObject.getXMidPos() < getXMidPos() )
                    xDirection = LEFT;
                else
                    xDirection = RIGHT;
            }

            if( std::abs(dx) < CSF_DISTANCE_TO_SHOOT )
            {
                mKeenNearby = true;
            }
        }
    }

    return true;
}

void CRoboRed::getTouchedBy(CSpriteObject &theObject)
{
    if(dead || theObject.dead)
        return;

    CStunnable::getTouchedBy(theObject);

    // Was it a bullet?
    if( dynamic_cast<CBullet*>(&theObject) && !getActionNumber(A_RED_SHOOT) )
    {
        setAction(A_RED_PAUSE);
    }

    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        player->kill();
    }
}


int CRoboRed::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CRoboRed::process()
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
