#include "CBipShip.h"
#include "CBip.h"

#include "engine/galaxy/common/ai/CPlayerLevel.h"
#include <engine/galaxy/common/ai/CBullet.h>


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
CGalaxyActionSpriteObject(pmap, foeID, x, y),
mTimer(0)
{
	mActionMap[A_BIPSHIP_MOVING] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processMoving;
	mActionMap[A_BIPSHIP_SHOT] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processTheShot;
	mActionMap[A_BIPSHIP_TURN] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processTurning;
	mActionMap[A_BIPSHIP_HIT] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processHit;
	mActionMap[A_BIPSHIP_CRASH] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processCrashing;
	mActionMap[A_BIPSHIP_WRECKAGE] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processWreckage;
	mActionMap[A_BIPSHIP_SMOKE] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processSmoke;
	
	setupGalaxyObjectOnMap(0x2A5C, A_BIPSHIP_MOVING);
	
	xDirection = LEFT;
}


void CBipShip::processMoving()
{
	// Move normally in the direction
	if( xDirection == RIGHT )
	{
		moveRight( FLY_SPEED );
	}
	else
	{
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


void CBipShip::processHit()
{    
    if(blockedd)
    {
	// If blooglet carried a gem it should fall that case it must jump out! That is a sprite item which can fall on the floor.
	const int newX = getXMidPos();
	const int newY = getYUpPos();
	auto *bip = new CBip(mp_Map, 0x6C, newX, newY);
	g_pBehaviorEngine->m_EventList.spawnObj( bip );
	setAction(A_BIPSHIP_CRASH);
	playSound(SOUND_BIP_SHIP_CRASH);	
    }
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

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction(A_BIPSHIP_HIT);
		dead = true;
		theObject.dead = true;
		playSound(SOUND_BIP_SHIP_LAND);		
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}

}


void CBipShip::process()
{
    if(dead)
    {
	
    }
    
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