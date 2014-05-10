/*
 * CSphereful.cpp
 *
 *  Created on: 02 Jan 2013
 *      Author: Gerstrong
 */


#include "CSphereful.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

/*
$303AW #Sphereful
$3058W #Sphereful
$3076W #Sphereful
$3094W #Sphereful
$30B2W #Sphereful
 */


namespace galaxy {  
  
enum SPARKYACTIONS
{
A_AMPTON_WALK = 0,
A_AMPTON_TURN = 4,
A_AMPTON_START_POLE = 5,
A_AMPTON_POLE_SLIDE = 6,
A_AMPTON_STOP_POLE = 7,
A_AMPTON_FLIP_SWITCH = 8,
A_AMPTON_STUNNED = 12
};

const int MOVE_SPEED = 10;

const int FLY_TIME = 150;

// TODO: Floating diamonds around are still missing!
  
CSphereful::CSphereful(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	m_ActionBaseOffset = 0x303A;
	
	setActionForce(0);
	setActionSprite();
	calcBoundingBoxes();
	
	yDirection = UP;
	xDirection = LEFT;
}



void CSphereful::processMoving()
{
  // Move normally in the direction
  moveXDir( xDirection*MOVE_SPEED );
  moveYDir( yDirection*MOVE_SPEED );
  
  if( blockedl && xDirection == LEFT )
  {
      playSound(SOUND_SPHEREFULCEILING);
      xDirection = RIGHT;      
  }
  else if(blockedr && xDirection == RIGHT)
  {
      playSound(SOUND_SPHEREFULCEILING);
      xDirection = LEFT;
  }
  
  if( blockedu && yDirection == UP )
  {
      playSound(SOUND_SPHEREFULCEILING);
      yDirection = DOWN;
  }
  else if(blockedd && yDirection == DOWN)
  {
      playSound(SOUND_SPINDREDFLYUP);
      yDirection = UP;
  }
  
}



bool CSphereful::isNearby(CSpriteObject &theObject)
{
    
    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
	
	mTimer++;
	if(mTimer < FLY_TIME)
	    return true;
	
	mTimer = 0;
	
	if( getProbability(600) )
	{
	    
	    if( player->getXMidPos() < getXMidPos() )
		xDirection = LEFT;
	    else
		xDirection = RIGHT;
	    
	    if(getProbability(700))
	    {
		yDirection = DOWN;
	    }
	    
	}		
    }
	
	
    return true;
}

void CSphereful::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}



void CSphereful::process()
{
	performCollisions();
	
	if(!processActionRoutine())
	    exists = false;
	
	processMoving();
}

}
