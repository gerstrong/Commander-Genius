/*
 * CWormmouth.cpp
 *
 *  Created on: 16.07.2012
 *      Author: gerstong
 */

#include "CWormmouth.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CBullet.h"

#include <base/interface/misc.h>
#include <engine/core/CBehaviorEngine.h>

namespace galaxy {


const int A_WORMMOUTH_MOVE = 0;
const int A_WORMMOUTH_LOOK = 1;
const int A_WORMMOUTH_EAT = 9;
const int A_WORMMOUTH_STUNNED = 14;

const int DIST_TO_EAT_X = (8<<STC);
const int DIST_TO_EAT_Y = 2<<CSF;
const int LOOK_TIMER = 10;

CWormmouth::CWormmouth(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y) :
  CStunnable(pMap, foeID, x, y),
  mTurnAround(false),
  mTimer(0)
{
  mActionMap[A_WORMMOUTH_MOVE]    = static_cast<GASOFctr>(&CWormmouth::processMoving);
  mActionMap[A_WORMMOUTH_LOOK]    = static_cast<GASOFctr>(&CWormmouth::processLooking);
  mActionMap[A_WORMMOUTH_EAT]     = static_cast<GASOFctr>(&CWormmouth::processEating);
  mActionMap[A_WORMMOUTH_STUNNED] = static_cast<GASOFctr>(&CStunnable::processGettingStunned);
  
  
  const Difficulty diff = gBehaviorEngine.mDifficulty;
  
  mHealthPoints = 1;
  
  if(foeID == 0x07 && diff > HARD)
    {
      // Set the slug to another color and double his health
      mSprVar = 1;
      mHealthPoints++;
    }
  if(foeID == 0x33 && diff > EXPERT)
    {
      // Set the slug to another color and increase his health
      mSprVar = 2;
      mHealthPoints++;
    }
  if(foeID == 0x34 && diff > NINJA)
    {
      // Set the slug to another color and increase his health
      mSprVar = 3;
      mHealthPoints++;
    }
  
  
  
  setupGalaxyObjectOnMap(0x26DE, A_WORMMOUTH_MOVE);
  processMove(0, -(1<<CSF));	
  processMove(0, (1<<CSF));	
  
  xDirection = LEFT;
}



bool CWormmouth::isNearby(CSpriteObject &theObject)
{
  if(mIsDead || theObject.mIsDead )
    return false;
  
  
  if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
      const int playerX = player->getXMidPos();
      const int playerY = player->getYMidPos();
      const int wormmouthX = getXMidPos();
      const int wormmouthY = getYMidPos();
      
      if( getActionNumber(A_WORMMOUTH_LOOK) )
      {
          if( playerX < wormmouthX && xDirection != LEFT )
              mTurnAround = true;
          else if( playerX > wormmouthX && xDirection != RIGHT )
              mTurnAround = true;
      }
      
      const int diffX = playerX - wormmouthX;
      const int diffY = playerY - wormmouthY;
      if( abs(diffX) < DIST_TO_EAT_X &&
          abs(diffY) < DIST_TO_EAT_Y &&
          !getActionNumber(A_WORMMOUTH_EAT) )
      {
          setAction(A_WORMMOUTH_EAT);
          playSound( GameSound::WORMOUTH_STRIKE );
      }
      
  }
  
  return true;
}




void CWormmouth::getTouchedBy(CSpriteObject &theObject)
{
    if(mIsDead || theObject.mIsDead )
        return;

    CStunnable::getTouchedBy(theObject);

    // Was it a bullet? Than make it stunned.
    if( dynamic_cast<CBullet*>(&theObject) )
    {
        setAction( A_WORMMOUTH_STUNNED );
        mIsDead = true;
        theObject.mIsDead = true;

        if(!processActionRoutine())
            exists = false;

        performCollisions();
        processGettingStunned();
    }


    if( !getActionNumber(A_WORMMOUTH_EAT) )
        return;


    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
        player->kill();
    }
}


void CWormmouth::processMoving()
{
  if( mTurnAround )
    {
      if( xDirection == LEFT )
	xDirection = RIGHT;
      else
	xDirection = LEFT;
      mTurnAround = false;
      setAction( A_WORMMOUTH_LOOK );
    }
  
  
  int lookTimer = LOOK_TIMER;
  
  const Difficulty diff = gBehaviorEngine.mDifficulty;
  
  if(diff > HARD)
    {
      lookTimer--;
    }
  if(diff > EXPERT)
    {
      lookTimer/=2;
    }
  
  if( mTimer < LOOK_TIMER )
    {
      mTimer++;
      return;
    }
  else
    {
      mTimer = 0;
    }
  
  if( getProbability(30) )
    setAction( A_WORMMOUTH_LOOK );
}

void CWormmouth::processLooking()
{
  if( getActionStatus(A_WORMMOUTH_MOVE) )
    setAction(A_WORMMOUTH_MOVE);
}

void CWormmouth::processEating()
{
  if( getActionStatus(A_WORMMOUTH_MOVE) )
    setAction(A_WORMMOUTH_MOVE);
}


int CWormmouth::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
  turnAroundOnCliff(x1, x2, y2 );
  
  return CSpriteObject::checkSolidD(x1, x2, y2, push_mode);   
}


void CWormmouth::process()
{
  if( getActionNumber(A_WORMMOUTH_MOVE) )
    {
      performCollisions();
      performGravityLow();
    }
  
  if(getActionNumber(A_WORMMOUTH_STUNNED))
    {
      performGravityHigh();
    }
  
  
  (this->*mp_processState)();
  
  if( getActionNumber(A_WORMMOUTH_STUNNED) )
    {
      return;
    }
  
  if( blockedl )
    {
      xDirection = RIGHT;
    }
  else if(blockedr)
    {
      xDirection = LEFT;
    }
  
  if(!processActionRoutine())
    {
      exists = false;
    }
  
}

} /* namespace galaxy */
