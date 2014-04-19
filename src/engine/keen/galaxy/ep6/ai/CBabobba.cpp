#include "CBabobba.h"

#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

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

const int SIT_TIME = 80;
const int NAP_TIME = 300;
const int SHOOT_TIME = 10;

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


const int CSF_DISTANCE_TO_FIRE = 7<<CSF;

CBabobba::CBabobba(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CStunnable(pmap, foeID, x, y),
mTimer(0),
mGoodFireChance(false)
{
	mActionMap[A_BABOBBA_JUMP] = (GASOFctr) &CBabobba::processJumping;
	mActionMap[A_BABOBBA_SIT] = (GASOFctr) &CBabobba::processSitting;
	mActionMap[A_BABOBBA_SHOOT] = (GASOFctr) &CBabobba::processShooting;
	mActionMap[A_BABOBBA_JUMP_ALT] = (GASOFctr) &CBabobba::processJumpingAlt;
	mActionMap[A_BABOBBA_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
	mActionMap[A_BABOBBA_NAPPING] = (GASOFctr) &CBabobba::processNapping;
	
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
	{
	    setAction(A_BABOBBA_SIT);
	    playSound(SOUND_BABOBBA_LAND);
	}
}

void CBabobba::processSitting()
{
    mTimer++;
    
    if(mTimer < SIT_TIME)
        return;
    
    mTimer = 0;        
    
    if(mGoodFireChance)
    {
	setAction(A_BABOBBA_SHOOT);
	playSound(SOUND_BABOBBA_CINDER);
	
	// Create cinder
	CCinder *cinder = new CCinder(mp_Map, 0, 
        getXMidPos(), getYUpPos(), xDirection, mSprVar);
	
	spawnObj( cinder );
    }    

    setAction(A_BABOBBA_NAPPING);
}

void CBabobba::processShooting()
{
    mTimer++;

    if(mTimer < SHOOT_TIME)
        return;
    
    mTimer = 0;        

    setAction(A_BABOBBA_SIT);
}

void CBabobba::processJumpingAlt()
{

}

void CBabobba::processNapping()
{
    mTimer++;

    if(mTimer < NAP_TIME)
        return;
    
    mTimer = 0;
    
    yinertia = MAX_JUMP_INERTIA;

    setAction(A_BABOBBA_JUMP);
    playSound(SOUND_BABOBBA_JUMP);
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
		
		const int objX = theObject.getXMidPos();
		const int objY = theObject.getYMidPos();
		const int babobbaX = getXMidPos();
		const int babobbaY = getYMidPos();
		
		mGoodFireChance = false;
		
		if( objX < babobbaX - CSF_DISTANCE_TO_FIRE ||
			objX > babobbaX + CSF_DISTANCE_TO_FIRE )
			return false;

		if( objY < babobbaY - CSF_DISTANCE_TO_FIRE ||
			objY > babobbaY + CSF_DISTANCE_TO_FIRE )
			return false;
		
		mGoodFireChance = true;
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
	
	if(getActionNumber(A_BABOBBA_NAPPING))
	    return;

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



/// Classes for cinder, which is some fire that land on the floor and vanishes

/*
$30CEW  #Babobba cinder tossed
$30ECW  #Babobba cinder landed
$310AW  #Babobba cinder dying
 */

enum CINDERACTIONS
{
A_CINDER_TOSSED = 0,
A_CINDER_LANDED = 1,
A_CINDER_DYING = 2
};


const int CINDER_X_SPEED = 50;
const int CINDER_DYING_TIME = 50;


CCinder::CCinder(CMap* pmap, const Uint16 foeID, 
         const Uint32 x, const Uint32 y, const int horDir, const int sprVar) :
CGalaxyActionSpriteObject(pmap, foeID, x, y, sprVar),
mTimer(0)
{
	mActionMap[A_CINDER_TOSSED] = (void (CGalaxyActionSpriteObject::*)()) &CCinder::processTossed;
	mActionMap[A_CINDER_LANDED] = (void (CGalaxyActionSpriteObject::*)()) &CCinder::processLanded;
	mActionMap[A_CINDER_DYING] = (void (CGalaxyActionSpriteObject::*)()) &CCinder::processDying;
	
	setupGalaxyObjectOnMap(0x30CE, A_CINDER_TOSSED);
	
	yinertia = -60;
	xDirection = horDir;
}



void CCinder::processTossed()
{
    // Move normally in the direction
    moveXDir(xDirection*CINDER_X_SPEED);
    
    if(blockedd)
    {
	setAction(A_CINDER_LANDED);
    }
}


void CCinder::processLanded()
{
    mTimer++;

    if(mTimer < CINDER_DYING_TIME)
        return;
    
    mTimer = 0;
   
    setAction(A_CINDER_DYING);
}


void CCinder::processDying()
{
    mTimer++;

    if(mTimer < CINDER_DYING_TIME)
        return;
    
    mTimer = 0;
    
    dead = true;
    exists = false;
}



void CCinder::getTouchedBy(CSpriteObject& theObject)
{
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
	player->kill();
    }
}



void CCinder::process()
{
	performCollisions();
	
	performGravityHigh();

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}



    
};
