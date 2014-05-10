/*
 * CSparky.cpp
 *
 *  Created on: 26 Dez 2012
 *      Author: Gerstrong
 */


#include "CSparky.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>


namespace galaxy {  
  
enum SPARKYACTIONS
{
A_SPARKY_WALK = 0,	/* Ordinary slug_move action */
A_SPARKY_LOOK = 4,
A_SPARKY_CHARGE = 12,
A_SPARKY_TURN = 20,
A_SPARKY_STUNNED = 23
};

const int TIME_UNTIL_MOVE = 5;
const int TIME_FOR_LOOK = 150;

const int WALK_SPEED = 25;

const int CSF_DISTANCE_TO_FOLLOW = 6<<CSF;

const int CHARGE_TIME = 250;
const int CHARGE_SPEED = 75;

const int TURN_TIME = 10;

  
CSparky::CSparky(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0),
mLookTimer(0),
mGoodChargeChance(false)

{
  
  //std::function<void()> do_walk = processWalking;
  //do_walk = processWalking;
  //do_walk.assign(CSparky::processWalking);
  
    mActionMap[A_SPARKY_WALK] = (void (CGalaxyActionSpriteObject::*)()) &CSparky::processWalking;
    mActionMap[A_SPARKY_LOOK] = (void (CGalaxyActionSpriteObject::*)()) &CSparky::processLook;
    mActionMap[A_SPARKY_CHARGE] = (void (CGalaxyActionSpriteObject::*)()) &CSparky::processCharge;
    mActionMap[A_SPARKY_TURN] = (void (CGalaxyActionSpriteObject::*)()) &CSparky::processTurn;
    mActionMap[A_SPARKY_STUNNED] = (void (CGalaxyActionSpriteObject::*)()) &CStunnable::processGettingStunned;
  
    auto diff = g_pBehaviorEngine->mDifficulty;


    if(diff > NINJA && foeID == 0x06)
    {
        mSprVar = 3;
        mHealthPoints = 4;
    }
    if(diff > EXPERT && foeID == 0x05)
    {
        mSprVar = 2;
        mHealthPoints = 3;
    }
    if(diff > HARD && foeID == 0x04)
    {
        mSprVar = 1;
        mHealthPoints = 2;
    }


    // Adapt this AI
    setupGalaxyObjectOnMap(0x1F0C, A_SPARKY_WALK);

    xDirection = LEFT;
}



void CSparky::processWalking()
{
  
  mLookTimer++;
  
  // Move normally in the direction
  if( xDirection == RIGHT )
  {
    moveRight( WALK_SPEED );
  }
  else
  {
    moveLeft( WALK_SPEED );
  }
   
  mTimer = 0;
  
  if(mLookTimer >= TIME_FOR_LOOK)
  {
    setAction(A_SPARKY_LOOK);
    mLookTimer = 0;
  }
}


void CSparky::processLook()
{    
  if(getActionStatus(A_SPARKY_WALK))
  {
    if(mGoodChargeChance)      
    {
      xDirection = mKeenAlignment;
      setAction(A_SPARKY_CHARGE);
      playSound(SOUND_SPARKY_CHARGE);
    }
    else if(mKeenAlignment != xDirection)
      setAction(A_SPARKY_TURN);
    else
      setAction(A_SPARKY_WALK);
  }
}

void CSparky::processCharge()
{
  mTimer++;
  
  // Move fast in the direction
  if( xDirection == RIGHT )
  {
    moveRight( CHARGE_SPEED );
  }
  else
  {
    moveLeft( CHARGE_SPEED );    
  }
  
  playSound(SOUND_KEEN_WALK);
  
  mTimer = 0;
  
  mLookTimer++;
  
  if(mLookTimer >= CHARGE_TIME)
  {
    setAction(A_SPARKY_WALK);
    mLookTimer = 0;
  }
  
}


void CSparky::processTurn()
{  
  mTimer++;
  
  if(mTimer < TURN_TIME)
    return;  
  
  mTimer = 0;
  
  setAction(A_SPARKY_WALK);
}


bool CSparky::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(10) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignment = LEFT;
		else
			mKeenAlignment = RIGHT;
		
		
		const int objX = theObject.getXMidPos();
		const int objY = theObject.getYMidPos();
		const int sparkyX = getXMidPos();
		const int sparkyY = getYMidPos();
		
		mGoodChargeChance = false;
		
		if( objX < sparkyX - CSF_DISTANCE_TO_FOLLOW ||
			objX > sparkyX + CSF_DISTANCE_TO_FOLLOW )
			return false;

		if( objY < sparkyY - CSF_DISTANCE_TO_FOLLOW ||
			objY > sparkyY + CSF_DISTANCE_TO_FOLLOW )
			return false;
		
		mGoodChargeChance = true;
	}

	return true;
}

void CSparky::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

    // Was it a bullet? Than make it stunned when health goes to zero.
	if( dynamic_cast<CBullet*>(&theObject) )
	{        
        mHealthPoints--;
        theObject.dead = true;

        if(mHealthPoints == 0)
        {
            setAction(A_SPARKY_STUNNED);
            dead = true;
        }
        else
        {
            blink(10);
        }
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


int CSparky::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
    if(turnAroundOnCliff( x1, x2, y2 ) && !dead)
        setAction(A_SPARKY_TURN);

    return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CSparky::process()
{
	performCollisions();
	
    performGravityMid();

	if(!dead) // If we is dead, there is no way to continue moving or turning
	{
	  if( blockedl )
	  {
	    if(xDirection == LEFT)
	      setAction(A_SPARKY_TURN);
	    
	    xDirection = RIGHT;
	  }
	  else if(blockedr)
	  {
	    if(xDirection == RIGHT)
	      setAction(A_SPARKY_TURN);
	    
	    xDirection = LEFT;
	  }
	}

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
