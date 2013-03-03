#include "CBipShip.h"

#include "engine/galaxy/common/ai/CPlayerLevel.h"


namespace galaxy
{

enum BIPSHIPACTIONS
{
A_BIPSHIP_MOVING = 0,	// Ordinary move action 
A_BIPSHIP_SHOT = 1,
A_BIPSHIP_TURN = 2,
A_BIPSHIP_HIT = 6,
A_BIPSHIP_CRASH = 7,
A_BIPSHIP_WRECKAGE = 8,
A_BIPSHIP_SMOKE = 9
};


const int FLY_SPEED = 30;


CBipShip::CBipShip(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	mActionMap[A_BIPSHIP_MOVING] = (void (CStunnable::*)()) &CBipShip::processMoving;
	mActionMap[A_BIPSHIP_SHOT] = (void (CStunnable::*)()) &CBipShip::processTheShot;
	mActionMap[A_BIPSHIP_TURN] = (void (CStunnable::*)()) &CBipShip::processTurning;
	mActionMap[A_BIPSHIP_HIT] = (void (CStunnable::*)()) &CBipShip::processHitting;
	mActionMap[A_BIPSHIP_CRASH] = (void (CStunnable::*)()) &CBipShip::processCrashing;
	mActionMap[A_BIPSHIP_WRECKAGE] = (void (CStunnable::*)()) &CBipShip::processWreckage;
	mActionMap[A_BIPSHIP_SMOKE] = (void (CStunnable::*)()) &CBipShip::processSmoke;
	
	setupGalaxyObjectOnMap(0x2A5C, A_BIPSHIP_MOVING);
	
	xDirection = LEFT;
}


void CBipShip::processMoving()
{
	// Move normally in the direction
	if( xDirection == RIGHT )
	{
		//moveRight( m_Action.velX );
		moveRight( FLY_SPEED );
	}
	else
	{
		//moveLeft( m_Action.velX );
		moveLeft( FLY_SPEED );
	}
	
	
	
	if( blockedl && xDirection == LEFT )
	{
	  xDirection = RIGHT;
	  setAction(A_BIPSHIP_TURN);
	}	
        else if( blockedr && xDirection == RIGHT )
	{
	  xDirection = LEFT;
	  setAction(A_BIPSHIP_TURN);
	}
}



void CBipShip::processTheShot()
{

}


void CBipShip::processTurning()
{
  if( getActionStatus(A_BIPSHIP_MOVING) )
  {    
    blockedl = blockedr = false;    
    setAction(A_BIPSHIP_MOVING);
  }  	

}


void CBipShip::processHitting()
{

}

void CBipShip::processCrashing()
{

}


void CBipShip::processWreckage()
{

}

void CBipShip::processSmoke()
{

}


bool CBipShip::isNearby(CSpriteObject& theObject)
{
  return true;
}

void CBipShip::getTouchedBy(CSpriteObject& theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction(A_BIPSHIP_CRASH);
		dead = true;
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}

}


void CBipShip::process()
{
	performCollisions();
		
	if(getActionNumber() >= A_BIPSHIP_HIT)
	{
	  performGravityHigh();
	}	

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}



};