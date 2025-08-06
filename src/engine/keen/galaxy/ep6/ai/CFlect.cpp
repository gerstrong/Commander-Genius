#include "CFlect.h"

#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"

namespace galaxy
{

   
enum FLECTACTIONS
{
  A_FLECT_TURN = 0,
  A_FLECT_WALK = 2,
  A_FLECT_STUN = 6
};
  

constexpr int WALK_SPEED = 30;
  
  
CFlect::CFlect(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y)
{
	mActionMap[A_FLECT_TURN] = (GASOFctr) &CFlect::processWalking;
	mActionMap[A_FLECT_WALK] = (GASOFctr) &CFlect::processTurning;
	mActionMap[A_FLECT_STUN] = (GASOFctr) &CStunnable::processGettingStunned;
	
	setupGalaxyObjectOnMap(0x2BA6, A_FLECT_TURN);
	
	xDirection = LEFT;
}

void CFlect::processTurning()
{
  if( getActionStatus(A_FLECT_WALK) )
  {
    xDirection = -xDirection;
    setAction(A_FLECT_WALK);
  }  	
}


void CFlect::processWalking()
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

void CFlect::getTouchedBy(CSpriteObject& theObject)
{
  if(mIsDead || theObject.mIsDead)
    return;	
  
  // Was it a bullet? Than make it stunned.
  if( auto *bullet = dynamic_cast<CBullet*>(&theObject) )
  {
    // TODO: Here it depends on the direction. If teeth are in the way, the shot must be reflected.
    // Flect can only be stun from above normally
    
    // If bullet comes from above
    if(bullet->yDirection > 0)
    {
      CStunnable::getTouchedBy(theObject);	    
      setAction(A_FLECT_STUN);
      mIsDead = true;
      bullet->mIsDead = true;
    }
    else
    {
      bullet->playSound( GameSound::SHOT_HIT );
      bullet->xDirection = -bullet->xDirection;	    
      xDirection = bullet->xDirection;
      bullet->moveXDir(xDirection<<CSF);
      bullet->reverse();
    }
  }
  
  if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
  {
     player->push(*this);
  }
}

bool CFlect::isNearby(CSpriteObject& theObject)
{
    if(getActionStatus(A_FLECT_STUN) || mIsDead)
      return false;
  
    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
	const int objX = player->getXMidPos();
	const int flectX = getXMidPos();

	if( ( objX <= flectX && xDirection > 0) ||
	    ( objX > flectX && xDirection < 0 ) )
	{
	  setAction(A_FLECT_TURN);
	}
    }
    return true;  
}


int CFlect::checkSolidD(int x1, int x2, int y2, const bool push_mode)
{
  turnAroundOnCliff( x1, x2, y2 );

  return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}

void CFlect::process()
{
	performCollisions();
	
	performGravityHigh();

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


};
