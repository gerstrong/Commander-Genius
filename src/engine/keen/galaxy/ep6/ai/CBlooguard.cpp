#include "CBlooguard.h"

#include "../../common/ai/CPlayerLevel.h"

namespace galaxy {
  
        
enum BLOOGACTIONS
{
A_BLOOGUARD_WALK = 0,	// Ordinary walk action 
A_BLOOGUARD_CLUBBING = 4,
A_BLOOGUARD_STUNNED = 8
};

const int WALK_SPEED = 30;

const int CSF_DISTANCE_TO_CLUB = 6<<CSF;

const int TIME_UNTIL_CLUB = 38;

CBlooguard::CBlooguard(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mHealth(3),
mTimer(0),
mGoodClubChance(false),
mStubPlayer(false)
{
    mActionMap[A_BLOOGUARD_WALK] = (GASOFctr) &CBlooguard::processWalking;
    mActionMap[A_BLOOGUARD_CLUBBING] = (GASOFctr) &CBlooguard::processClubbing;
    mActionMap[A_BLOOGUARD_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
    
    setupGalaxyObjectOnMap(0x1F7C, A_BLOOGUARD_WALK);
    
    xDirection = LEFT;
}

void CBlooguard::processWalking()
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
    if( mTimer < TIME_UNTIL_CLUB )
        return;
    
    mTimer = 0;
    
    if(mGoodClubChance)
    {
        setAction(A_BLOOGUARD_CLUBBING);
    }
}

void CBlooguard::processClubbing()
{
    if( getActionStatus(A_BLOOGUARD_WALK) )
    {
        // At this point Keen must get stunned!
        mStubPlayer = true;
        setAction(A_BLOOGUARD_WALK);
        playSound(SOUND_BLOOGGUARD_STUB);
    }
}

int CBlooguard::checkSolidD(int x1, int x2, int y2, const bool push_mode)
{
    turnAroundOnCliff( x1, x2, y2 );
    
    return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}

bool CBlooguard::isNearby(CSpriteObject& theObject)
{
    if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
    {
		const int objX = theObject.getXMidPos();
		const int objY = theObject.getYMidPos();
		const int blooguardX = getXMidPos();
		const int blooguardY = getYMidPos();
		
		// Code for setting player stunned here!
		if(mStubPlayer)
		{
            mStubPlayer = false;
		    if(player->stun())
		    {
                mp_Map->triggerShake( 10, 5 );
            }
		    
		    return true;
		}
		
		mGoodClubChance = false;
		
		if( objX < blooguardX - CSF_DISTANCE_TO_CLUB ||
			objX > blooguardX + CSF_DISTANCE_TO_CLUB )
			return false;

		if( objY < blooguardY - CSF_DISTANCE_TO_CLUB ||
			objY > blooguardY + CSF_DISTANCE_TO_CLUB )
			return false;
		
		mGoodClubChance = true;
    }
    return true;
}


void CBlooguard::getTouchedBy(CSpriteObject& theObject)
{
    if(dead || theObject.dead)
	return;
    
    CStunnable::getTouchedBy(theObject);
    
    // Was it a bullet? Then loose health.
    if( dynamic_cast<CBullet*>(&theObject) )
    {
	mHealth--;
	theObject.dead = true;
	
	if(mHealth == 0)
	{
	    setAction(A_BLOOGUARD_STUNNED);
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

void CBlooguard::process()
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
