/*
 * CSphereful.cpp
 *
 *  Created on: 02 Jan 2013
 *      Author: Gerstrong
 */


#include "CSphereful.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

/* Actions addresses
$303AW #Sphereful
$3058W #Sphereful
$3076W #Sphereful
$3094W #Sphereful
$30B2W #Sphereful
 */


namespace galaxy {  
  

enum SPHEREFULACTIONS
{
    A_SPHEREFUL_FLY = 0,
    A_SPHEREFUL_STUN = 1,
    A_SPHEREFUL_STATE3 = 2,
    A_SPHEREFUL_STATE4 = 4,
    A_SPHEREFUL_STATE5 = 5,
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

    loadPythonScripts("sphereful");
}



void CSphereful::processMoving()
{
    // Keen 9: If this foe is stunnable. Make it possible to fall and don't make anything else
    if(dead)
    {
        performCollisions();

        return;
    }

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

    if(!mInvincible)
    {
        // Was it a bullet? Than make it stunned.
        if( dynamic_cast<CBullet*>(&theObject) )
        {
            dead = true;
            theObject.dead = true;
            setAction(A_SPHEREFUL_STUN);
        }
    }

    
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
        player->kill(false);
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
