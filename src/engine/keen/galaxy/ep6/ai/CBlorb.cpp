
#include "CBlorb.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"

namespace galaxy {  


constexpr int MOVE_SPEED = 10;
  
CBlorb::CBlorb(std::shared_ptr<CMap> pMap, const Uint16 foeID, 
		       const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y)
{
	mActionMap[0] = (GASOFctr) &CBlorb::processMoving;
	
	setupGalaxyObjectOnMap(0x3164, 0);
	
	xDirection = LEFT;
	yDirection = DOWN;
}


void CBlorb::processMoving()
{
	// Move normally in the direction
	moveXDir( xDirection*MOVE_SPEED );
	moveYDir( yDirection*MOVE_SPEED );	
	
	if( blockedl && xDirection == LEFT )
	{
	  xDirection = RIGHT;
	}
	else if(blockedr && xDirection == RIGHT)
	{
	  xDirection = LEFT;
	}

	if( blockedu && yDirection == UP )
	{
	  yDirection = DOWN;
	}
	else if(blockedd && yDirection == DOWN)
	{
	  yDirection = UP;
	}
}


void CBlorb::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.mIsDead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
        player->kill(false);
	}
}


void CBlorb::process()
{
	performCollisions();
	
	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();	
}

}
