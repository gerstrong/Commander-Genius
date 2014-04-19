/*
 * CSpindred.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CSpindred.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

/*
$2A7CW #Spindred
$2A9AW #Spindred
$2AB8W #Spindred
$2AD6W #Spindred
 */


namespace galaxy {  

const int WALK_SPEED = 25;

const int MAX_BOUNCES = 2;

const int MAX_BOUNCE_INERTIA = 120;

CSpindred::CSpindred(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0),
mInverseGravity(false),
mBounces(0)
{
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x2A7C, 0);
	
	xDirection = LEFT;
}


void CSpindred::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	/*if( dynamic_cast<CBullet*>(&theObject) )
	{
		playSound(SOUND_ROBO_STUN);
		dead = true;
		theObject.dead = true;
	}*/

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


void CSpindred::process()
{
	performCollisions();

	if(mInverseGravity)
	{
	    performInverseGravityHigh();
	}
	else
	{
	    performGravityHigh();
	}
	
	if(yinertia == 0)
	{	    
	    if( (blockedu && mInverseGravity) || 
		(blockedd && !mInverseGravity) )
	    {
		mBounces++;
		yinertia = (mInverseGravity) ? MAX_BOUNCE_INERTIA : -MAX_BOUNCE_INERTIA;
	    }
	    
	    
	    if(mBounces >= MAX_BOUNCES)
	    {
		mBounces = 0;
		mInverseGravity = !mInverseGravity;
	    }	    
	}
	
	processActionRoutine();	
	
}

}
