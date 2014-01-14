#include "CBip.h"

#include "../../common/ai/CPlayerLevel.h"

namespace galaxy
{    
            
enum BIPACTIONS
{
A_BIP_STAND = 0,
A_BIP_WALK = 1,	/* Ordinary walk action */
A_BIP_SQUISHED = 5
};
    
    
    
const int WALK_SPEED = 30;

const int STAND_TIME = 30;
const int WALK_TIME = 300;
    
    
CBip::CBip(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CGalaxyActionSpriteObject(pmap, foeID, x, y, 0),
mTimer(0)
{
    	mActionMap[A_BIP_STAND] = (void (CGalaxyActionSpriteObject::*)()) &CBip::processStanding;
    	mActionMap[A_BIP_WALK] = (void (CGalaxyActionSpriteObject::*)()) &CBip::processWalking;
    	mActionMap[A_BIP_SQUISHED] = (void (CGalaxyActionSpriteObject::*)()) &CBip::processSquished;
	
	setupGalaxyObjectOnMap(0x29A8, A_BIP_STAND);
	
	xDirection = LEFT;
}


void CBip::processStanding()
{
  mTimer++;
  
  if(getActionStatus(A_BIP_WALK))
      setAction(A_BIP_WALK);
    
  mTimer = 0;
}


void CBip::processWalking()
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
    
  mTimer++;
  
  if(mTimer < WALK_TIME)
    return;  
  
  mTimer = 0;
  setAction(A_BIP_STAND);
	
}


void CBip::processSquished()
{
    
}


void CBip::getTouchedBy(CSpriteObject& theObject)
{
	if(dead || theObject.dead)
		return;

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		playSound(SOUND_SQUISH);
		setAction(A_BIP_SQUISHED);
		dead = true;
	}
}



int CBip::checkSolidD(int x1, int x2, int y2, const bool push_mode)
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}

void CBip::process()
{
	performCollisions();
	
	performGravityHigh();

	if( blockedl )
		xDirection = RIGHT;
	else if(blockedr)
		xDirection = LEFT;

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}
    
    
    
};
