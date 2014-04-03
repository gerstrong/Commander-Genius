/*
 * CBloog.cpp
 *
 *  Created on: 26 Dez 2012
 *      Author: Gerstrong
 */


#include "CBloog.h"
#include "../../common/ai/CPlayerBase.h"


namespace galaxy {
  
enum BLOOGACTIONS
{
A_BLOOG_WALK = 0,	/* Ordinary walk action */
A_BLOOG_STUNNED = 4
};

const int WALK_SPEED = 30;


CBloog::CBloog(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{

    mHealthPoints = 1;

    const Difficulty diff = g_pBehaviorEngine->mDifficulty;

    if(foeID == 0x04 && diff > HARD)
    {
        // Set the bloog to another color and double his health
        mSprVar = 1;
        mHealthPoints *= 2;
    }
    if(foeID == 0x05 && diff > EXPERT)
    {
        // Set the bloog to another color and increase his health
        mSprVar = 2;
        mHealthPoints *= 3;
    }
    if(foeID == 0x06 && diff > NINJA)
    {
        // Set the bloog to another color and increase his health
        mSprVar = 3;
        mHealthPoints *= 4;
    }


	mActionMap[A_BLOOG_WALK] = (GASOFctr) &CBloog::processWalking;
	mActionMap[A_BLOOG_STUNNED] = (GASOFctr) &CStunnable::processGettingStunned;
	
	setupGalaxyObjectOnMap(0x1EE6, A_BLOOG_WALK);
	
	xDirection = LEFT;

    // Some mods suffer the fact (Keen 8 Dead in Desert) that those foes appear embedded in the floor
    GsSprite &rSprite = gGraphics.getSprite(mSprVar,sprite);
    performCollisions();
    processMove( 0, rSprite.m_bboxY1-rSprite.m_bboxY2 );
    processActionRoutine();
}



void CBloog::processWalking()
{
	// Move normally in the direction
	if( xDirection == RIGHT )
	{
		//moveRight( m_Action.velX );
		moveRight( WALK_SPEED );
	}
	else
	{
		//moveLeft( m_Action.velX );
		moveLeft( WALK_SPEED );
	}
}


void CBloog::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{		
        mHealthPoints--;
        theObject.dead = true;

        if(mHealthPoints == 0)
        {
            setAction(A_BLOOG_STUNNED);
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


int CBloog::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
    turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CBloog::process()
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

}
