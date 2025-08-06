/*
 * CSphereful.cpp
 *
 *  Created on: 02 Jan 2013
 *      Author: Gerstrong
 */


#include "CSphereful.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include <base/interface/misc.h>

/* Actions addresses
$303AW #Sphereful
$3058W #Sphereful
$3076W #Sphereful
$3094W #Sphereful
$30B2W #Sphereful
 */


namespace galaxy {  
  

enum SPHEREFULACTIONS
{
    A_SPHEREFUL_FLY = 0,
    A_SPHEREFUL_STATE2 = 1,
    A_SPHEREFUL_STATE3 = 2,
    A_SPHEREFUL_STATE4 = 4,
    A_SPHEREFUL_STATE5 = 5,
};

constexpr int MOVE_SPEED = 10;
constexpr int FLY_TIME = 150;

// TODO: Floating diamonds around are still missing!
  
CSphereful::CSphereful(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y),
mTimer(0)
{
	m_ActionBaseOffset = 0x303A;

    mActionMap[A_SPHEREFUL_FLY] = (GASOFctr) &CSphereful::processMoving;
    mActionMap[A_SPHEREFUL_STATE2] = (GASOFctr) &CSphereful::processMoving;
    mActionMap[A_SPHEREFUL_STATE3] = (GASOFctr) &CSphereful::processMoving;
    mActionMap[A_SPHEREFUL_STATE4] = (GASOFctr) &CSphereful::processMoving;
    mActionMap[A_SPHEREFUL_STATE5] = (GASOFctr) &CSphereful::processMoving;

	
	setActionForce(0);
	setActionSprite();
	calcBoundingBoxes();
	
	yDirection = UP;
	xDirection = LEFT;

    loadLuaScript("sphereful");

    if(!mInvincible)
    {
        mActionMap[A_SPHEREFUL_STATE2] = (GASOFctr) &CSphereful::processStun;
        mActionMap[A_SPHEREFUL_STATE3] = (GASOFctr) &CSphereful::processStun;
        mActionMap[A_SPHEREFUL_STATE4] = (GASOFctr) &CSphereful::processStun;
        mActionMap[A_SPHEREFUL_STATE5] = (GASOFctr) &CSphereful::processStun;
    }
}



void CSphereful::processStun()
{
    performCollisions();
    performGravityMid();
}

void CSphereful::processMoving()
{
  // Move normally in the direction
  moveXDir( xDirection*MOVE_SPEED );
  moveYDir( yDirection*MOVE_SPEED );
  
  if( blockedl && xDirection == LEFT )
  {
      playSound(GameSound::SPHEREFULCEILING);
      xDirection = RIGHT;      
  }
  else if(blockedr && xDirection == RIGHT)
  {
      playSound(GameSound::SPHEREFULCEILING);
      xDirection = LEFT;
  }
  
  if( blockedu && yDirection == UP )
  {
      playSound(GameSound::SPHEREFULCEILING);
      yDirection = DOWN;
  }
  else if(blockedd && yDirection == DOWN)
  {
      playSound(GameSound::SPINDREDFLYUP);
      yDirection = UP;
  }
  
}



bool CSphereful::isNearby(CSpriteObject &theObject)
{

    
    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
        mTimer++;
        if(mTimer < FLY_TIME)
            return true;

        mTimer = 0;

        if( getProbability(600) )
        {

            if( player->getXMidPos() < getXMidPos() )
                xDirection = LEFT;
            else
                xDirection = RIGHT;

            if(getProbability(700))
            {
                yDirection = DOWN;
            }

        }
    }

	
    return true;
}

void CSphereful::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.mIsDead = true;

        if(!mInvincible)
        {
            mIsDead = true;

            setAction(A_SPHEREFUL_STATE3);
        }
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->kill(false);
	}
}



void CSphereful::process()
{
	performCollisions();
	
    if(!mIsDead)
    {
        if(!processActionRoutine())
        {
            exists = false;
        }
    }
	
    (this->*mp_processState)();
}

}
