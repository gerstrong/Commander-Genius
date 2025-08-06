#include "CBipShip.h"
#include "CBip.h"

#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include "../../common/ai/CEnemyShot.h"


namespace galaxy
{

enum BIPSHIPACTIONS
{
A_BIPSHIP_MOVING = 0,	// Ordinary move action 
A_BIPSHIP_TURN = 2,
A_BIPSHIP_HIT = 6,
A_BIPSHIP_CRASH = 7,
A_BIPSHIP_WRECKAGE = 8,
A_BIPSHIP_SMOKE = 9
};


constexpr int FLY_SPEED = 30;

constexpr int TIME_UNTIL_LOOK = 160;
constexpr int CSF_DISTANCE_TO_SHOOT = 8<<CSF;



CBipShip::CBipShip(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int sprVar) :
CGalaxyActionSpriteObject(pMap, foeID, x, y, sprVar),
mTimer(0),
mKeenIsNear(false)
{
	mActionMap[A_BIPSHIP_MOVING] = (void (CGalaxyActionSpriteObject::*)()) &CBipShip::processMoving;
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
	
	if(mKeenIsNear)
	{
        playSound(GameSound::BIPSHIP_SHOOT);
		int x_coord = getXMidPos();
		x_coord += (xDirection == LEFT) ? -(8<<STC) : +(8<<STC);

	  	CEnemyShot *laser = new CEnemyShot(mpMap, 0, x_coord, getYMidPos()-(8<<STC),
                                    0x2A7A, xDirection, 0,  100, mSprVar);
		spawnObj( laser );
		mKeenIsNear = false; 
	}
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
	// crash and spawn bip
	const int newX = getXMidPos();
	const int newY = getYUpPos();
	auto *bip = new CBip(mpMap, 0x6C, newX, newY);
	spawnObj( bip );
	setAction(A_BIPSHIP_WRECKAGE);
    }
}

void CBipShip::processCrashing()
{
    
}


void CBipShip::processWreckage()
{
  if(blockedd)
  {
    setAction(A_BIPSHIP_CRASH);
    playSound(GameSound::BIP_SHIP_CRASH);
  }
}

void CBipShip::processSmoke()
{

}


bool CBipShip::isNearby(CSpriteObject& theObject)
{
  if( dynamic_cast<CPlayerLevel*>(&theObject) )
  {
    mTimer++;
    if(mTimer < TIME_UNTIL_LOOK)
      return true;
    
    mTimer = 0;	    
    
    mKeenIsNear = false;
    
    const int objX = theObject.getXMidPos();
    const int shipX = getXMidPos();
    const int dx = objX - shipX;
    
    if( theObject.getYDownPos() > getYUpPos() && theObject.getYUpPos() < getYDownPos() )
    {
      if( std::abs(dx) < CSF_DISTANCE_TO_SHOOT )
      {
	mKeenIsNear = true;
      }
    }
  }  
  
  return true;
}

int CBipShip::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
    turnAroundOnCliff( x1, x2, y2 );

    return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CBipShip::getTouchedBy(CSpriteObject& theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction(A_BIPSHIP_HIT);
		mIsDead = true;
		theObject.mIsDead = true;
        playSound(GameSound::BIP_SHIP_LAND);
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}

}


void CBipShip::process()
{
    performCollisions();

    performGravityHigh();


	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}



};
