#include "CBabobba.h"
#include <engine/galaxy/common/ai/CPlayerBase.h>
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include <misc.h>

namespace galaxy 
{
  
/*
$2F48W  #Babobba jump 0
$2F66W  #Babobba jump
$2F84W  #Babobba sit 2
$2FA2W  #Babobba sit [shoot] 3
$2FC0W  #Babobba jump? 4
$2FDEW  #Babobba stunned 5
$2FFCW  #Babobba napping 6
$301AW  #Babobba napping
$3038W  #Babobba napping
$3056W  #Babobba napping [Asleep]
$3074W  #Babobba napping
$3092W  #Babobba napping
$30B0W  #Babobba napping
*/    

const int MOVE_X_SPEED = 30;

const int SIT_TIME = 60;

const int MAX_JUMP_INERTIA = -120;


enum BABOBBAACTIONS
{
A_BABOBBA_JUMP = 0,
A_BABOBBA_SIT = 2,
A_BABOBBA_SHOOT = 3,
A_BABOBBA_JUMP_ALT = 4,
A_BABOBBA_STUNNED = 5,
A_BABOBBA_NAPPING = 6
};

CBabobba::CBabobba(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
	mActionMap[A_BABOBBA_JUMP] = (void (CStunnable::*)()) &CBabobba::processJumping;
	mActionMap[A_BABOBBA_SIT] = (void (CStunnable::*)()) &CBabobba::processSitting;
	mActionMap[A_BABOBBA_SHOOT] = (void (CStunnable::*)()) &CBabobba::processShooting;
	mActionMap[A_BABOBBA_JUMP_ALT] = (void (CStunnable::*)()) &CBabobba::processJumpingAlt;
	mActionMap[A_BABOBBA_STUNNED] = &CStunnable::processGettingStunned;
	mActionMap[A_BABOBBA_NAPPING] = (void (CStunnable::*)()) &CBabobba::processNapping;
	
	setupGalaxyObjectOnMap(0x2F48, A_BABOBBA_JUMP);
	
	xDirection = LEFT;
	yinertia = MAX_JUMP_INERTIA;
}


void CBabobba::processJumping()
{
	// Move normally in the direction
	if( xDirection == RIGHT )
	{
		moveRight( MOVE_X_SPEED );
	}
	else
	{
		moveLeft( MOVE_X_SPEED );
	}
	
	if(blockedd && yinertia >= 0)
	    setAction(A_BABOBBA_SIT);
}

void CBabobba::processSitting()
{
    mTimer++;

    if(mTimer < SIT_TIME)
	return;
    
    mTimer = 0;
    
    yinertia = MAX_JUMP_INERTIA;
    
    setAction(A_BABOBBA_JUMP);
}

void CBabobba::processShooting()
{

}

void CBabobba::processJumpingAlt()
{

}

void CBabobba::processNapping()
{

}

bool CBabobba::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(30) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			xDirection = LEFT;
		else
			xDirection = RIGHT;
	}

	return true;
}

void CBabobba::getTouchedBy(CSpriteObject& theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
		setAction(A_BABOBBA_STUNNED);
		dead = true;
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


void CBabobba::process()
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



/// TODO: Classes for cinder

/*
$30CEW  #Babobba cinder tossed
$30ECW  #Babobba cinder landed
$310AW  #Babobba cinder dying
$3128W  #Babobba cinder dying
$3146W  #Babobba cinder vannish 
 */

    
};