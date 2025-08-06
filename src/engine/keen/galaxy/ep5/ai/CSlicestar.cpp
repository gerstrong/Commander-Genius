/*
 * CSlicestar.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CSlicestar.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"


namespace galaxy {  
  
enum SLICESTARACTIONS
{
A_SLICESTAR_MOVE = 0
};

constexpr int MOVE_SPEED = 25;
  
CSlicestar::CSlicestar(std::shared_ptr<CMap> pMap, const Uint16 foeID, 
		       const Uint32 x, const Uint32 y, 
		       const bool useBlocker, direction_t horDir, 
		       direction_t verDir) :
CStunnable(pMap, foeID, x, y),
mUseBlocker(useBlocker)
{
	m_ActionBaseOffset = 0x23BC;
	
	xDirection = horDir;
	yDirection = verDir;
	
	setActionForce(A_SLICESTAR_MOVE);
	setActionSprite();
	calcBoundingBoxes();
}



void CSlicestar::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

    // Was it a bullet? Zap zot!
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.mIsDead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->kill(false);
	}
}


void CSlicestar::process()
{
	performCollisions();

	if(mUseBlocker)
	{
	  const Uint16 object = mpMap->getPlaneDataAt(2, getPosition());
	
	  // If there is a blocker, change the direction
	  if( object == 0x1F )
	  {
	    if(xDirection != CENTER)
	      xDirection = -xDirection;
	      
	    if(yDirection != CENTER)
	      yDirection = -yDirection;
	  }

	}

	// Move normally in the direction
	moveXDir( xDirection*MOVE_SPEED );
	moveYDir( yDirection*MOVE_SPEED );	
	
	if( blockedl && xDirection == LEFT )
	{
      playSound(GameSound::SLICEBUMP);
	  xDirection = RIGHT;
	}
	else if(blockedr && xDirection == RIGHT)
	{
      playSound(GameSound::SLICEBUMP);
	  xDirection = LEFT;
	}

	if( blockedu && yDirection == UP )
	{
      playSound(GameSound::SLICEBUMP);
	  yDirection = DOWN;
	}
	else if(blockedd && yDirection == DOWN)
	{
      playSound(GameSound::SLICEBUMP);
	  yDirection = UP;
	}
}

}
