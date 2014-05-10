/*
 * CShelly.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShelly.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

/*
$2416W #Shelly walk
$2434W #Shelly walk
$2452W #Shelly walk
$2470W #Shelly walk
$248EW #Shelly prepare to jump 4
$24ACW #Shelly jump 5
$24CAW #Shelly fall 6
$24E8W #Shelly fall
$2506W #Shelly smash 8
$2524W #Shelly smash
$2542W #Shelly smash smoke 10
$2560W #Shelly smash smoke
$257EW #Shelly smash smoke
$259CW #Shelly smash smoke
$25BAW #Shelly fragments 14
$25D8W #Shelly fragments 15 -> TODO: Need to be other actions!
 */


namespace galaxy {  
  
enum SHELLYACTIONS
{
A_SHELLY_WALK = 0,
A_SHELLY_PREPARE_JUMP = 4,
A_SHELLY_JUMP = 5,
A_SHELLY_FALL = 6,
A_SHELLY_SMASH = 8,
A_SHELLY_SMOKE = 10
};

const int WALK_SPEED = 10;

const int JUMP_X_SPEED = 50;

const int SMOKE_TIME = 40;

const int MAX_JUMP_INERTIA = 150;

const int CSF_DISTANCE_TO_JUMP = 6<<CSF;
  
CShelly::CShelly(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
      	mActionMap[A_SHELLY_WALK] = (GASOFctr) &CShelly::processWalking;
      	mActionMap[A_SHELLY_PREPARE_JUMP] = (GASOFctr) &CShelly::processPrepareJump;
      	mActionMap[A_SHELLY_JUMP] = (GASOFctr) &CShelly::processJump;
      	mActionMap[A_SHELLY_FALL] = (GASOFctr) &CShelly::processFall;
      	mActionMap[A_SHELLY_SMASH] = (GASOFctr) &CShelly::processSmash;
      	mActionMap[A_SHELLY_SMOKE] = (GASOFctr) &CShelly::processSmoke;
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2416, A_SHELLY_WALK);
	
	xDirection = LEFT;
}



void CShelly::processWalking()
{ 
    
    if( blockedl )
    {
	xDirection = RIGHT;
    }
    else if(blockedr)
    {
	xDirection = LEFT;
    }
    
    // Move normally in the direction
    moveXDir(xDirection*WALK_SPEED);   
        
    if(mGoodJumpChance)
    {
	setAction(A_SHELLY_PREPARE_JUMP);
    }
}


void CShelly::processPrepareJump()
{
    if(getActionStatus(A_SHELLY_JUMP))
    {
	if(xDirection != mKeenAlignment)
	{
	    xDirection = mKeenAlignment;
	    setAction(A_SHELLY_WALK);
	    return;
	}
	
	if(mGoodJumpChance)
	{
	    setAction(A_SHELLY_JUMP);
	    yinertia = -MAX_JUMP_INERTIA;
	}
	else
	{
	    setAction(A_SHELLY_WALK);
	}
    }
}


void CShelly::processJump()
{
    if( getActionStatus(A_SHELLY_WALK) )
    {
	setAction(A_SHELLY_FALL);
    }
    
    if( getActionStatus(A_SHELLY_FALL)  )
    {
	setAction(A_SHELLY_FALL);
    }
}


void CShelly::processFall()
{

    moveXDir(xDirection*JUMP_X_SPEED);
    
    if(blockedd)
	setAction(A_SHELLY_SMASH);
}


void CShelly::processSmash()
{
    if( blockedd )
	setAction(A_SHELLY_SMOKE);
}


void CShelly::processSmoke()
{
    mTimer++;
    
    if(mTimer < SMOKE_TIME)
	return;
    
    mTimer = 0;
    
    dead = true;
      
    // Spawn little explosion shards here!
    const int newX = getXMidPos();      
    const int newY = getYUpPos();
      
    spawnObj( new CShellyFrags( getMapPtr(), 
							     0, newX, newY, -100 ) );
    spawnObj( new CShellyFrags( getMapPtr(), 
							     0, newX, newY, -50 ) );
    spawnObj( new CShellyFrags( getMapPtr(), 
							     0, newX, newY, 50 ) );
    spawnObj( new CShellyFrags( getMapPtr(), 
							     0, newX, newY, 100 ) );
    exists = false;
}


bool CShelly::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignment = LEFT;
		else
			mKeenAlignment = RIGHT;
		
		
		const int objX = theObject.getXMidPos();
		const int objY = theObject.getYMidPos();
		const int shellyX = getXMidPos();
		const int shellyY = getYMidPos();
		
		mGoodJumpChance = false;
		
		if( objX < shellyX - CSF_DISTANCE_TO_JUMP ||
			objX > shellyX + CSF_DISTANCE_TO_JUMP )
			return false;

		if( objY < shellyY - CSF_DISTANCE_TO_JUMP ||
			objY > shellyY + CSF_DISTANCE_TO_JUMP )
			return false;
		
		mGoodJumpChance = true;
	}
	
	return true;
}

void CShelly::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? 
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


int CShelly::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CShelly::process()
{
	performCollisions();
	
	performGravityHigh();

	processActionRoutine();
	
	(this->*mp_processState)();
}


////////////////////////
/// Shelly fragments ///
////////////////////////

CShellyFrags::CShellyFrags(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xSpeed) :
CStunnable(pmap, foeID, x, y),
mXSpeed(xSpeed)
{
  xDirection = (xSpeed < 0) ? LEFT : RIGHT;
    
  setupGalaxyObjectOnMap(0x25BA, 0);
  
  yinertia = -100;
}


void CShellyFrags::getTouchedBy(CSpriteObject& theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}		
}




void CShellyFrags::process()
{
	performCollisions();
	
	performGravityMid();

	// Reflections at wall. Not sure, if it makes sense, let's observe the behaviour
	if( (blockedl && mXSpeed < 0) ||
	    (blockedr && mXSpeed > 0) )
	{
	  mXSpeed = -mXSpeed;
	}		
	
	if( blockedd )
	{
	  dead = true;
	  exists = false;
	  return;
	}
	
	
	moveXDir(mXSpeed);
	
	processActionRoutine();	
}


}
