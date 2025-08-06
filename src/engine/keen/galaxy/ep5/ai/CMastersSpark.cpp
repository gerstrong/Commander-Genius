/*
 * CMastersSpark.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CMastersSpark.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/interface/misc.h>

/*
$2C98W #Master sparks
$2CB6W #Master sparks
$2CD4W #Master sparks
$2CF2W #Master sparks
 */


namespace galaxy {  

constexpr int WALK_SPEED = 25;

  
CMastersSpark::CMastersSpark(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y)
{
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2C98, 0);
	
	xDirection = LEFT;
}



void CMastersSpark::processWalking()
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


void CMastersSpark::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->kill(false);
	}
}

void CMastersSpark::process()
{
	performCollisions();
	
	performGravityMid();

	if( blockedl )
	{
	  xDirection = RIGHT;
	  mIsDead = true;
	}
	else if(blockedr)
	{
	  xDirection = LEFT;
	  mIsDead = true;
	}

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
