/*
 * CPoisonSlug.cpp
 *
 *  Created on: 23FEB2011
 *      Author: FCTW
 */


#include "CPoisonSlug.h"
#include "CSlugSlime.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CBullet.h"

#include <engine/core/CBehaviorEngine.h>
#include <base/interface/misc.h>

namespace galaxy {


enum SLUGACTIONS
{
A_SLUG_MOVE = 0,	/* Ordinary slug_move action */
A_SLUG_POOING = 2,
A_SLUG_STUNNED = 3,
A_SLUG_STUNNED_ALT = 4
};

//const int SLUG_MOVE_SPEED = 1;
const int SLUG_MOVE_TIMER = 8;

CPoisonSlug::CPoisonSlug(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y) :
CStunnable(pMap, foeID, x, y),
m_timer(0)
{
    const Difficulty diff = gBehaviorEngine.mDifficulty;

    mHealthPoints = 1;

    if(foeID == 0x16 && diff > HARD)
    {
        // Set the slug to another color and double his health
        mSprVar = 1;
        mHealthPoints *= 2;
    }
    if(foeID == 0x2B && diff > EXPERT)
    {
        // Set the slug to another color and increase his health
        mSprVar = 2;
        mHealthPoints *= 3;
    }
    if(foeID == 0x2C && diff > NINJA)
    {
        // Set the slug to another color and increase his health
        mSprVar = 3;
        mHealthPoints *= 4;
    }

    mapFunction(A_SLUG_MOVE, &CPoisonSlug::processCrawling);
    mapFunction(A_SLUG_POOING, &CPoisonSlug::processPooing);
    mapFunction(A_SLUG_STUNNED, &CPoisonSlug::processGettingStunned);
    mapFunction(A_SLUG_STUNNED_ALT, &CPoisonSlug::processGettingStunned);

	setupGalaxyObjectOnMap(0x2012, A_SLUG_MOVE);

	xDirection = LEFT;
}



void CPoisonSlug::processCrawling()
{

    if( m_timer < SLUG_MOVE_TIMER )
    {
        m_timer++;
        return;
    }
    else
    {
        m_timer = 0;
    }

    // Chance to poo
    if( getProbability(10) )
    {
        m_timer = 0;
        setAction( A_SLUG_POOING );
        playSound( GameSound::SLUG_DEFECATE );
        CSlugSlime *slime = new CSlugSlime(mpMap, 0,
                                           getXMidPos(),
                                           getYDownPos()-(1<<(CSF-1)), 0);
        gEventManager.add( new EventSpawnObject( slime ) );

        xDirection = -xDirection;
        return;
    }

    // Move normally in the direction
    if( xDirection == RIGHT )
    {
        moveRight( 2*m_Action.h_anim_move );
    }
    else
    {
        moveLeft( 2*m_Action.h_anim_move );
    }
}





void CPoisonSlug::processPooing()
{
	if( getActionStatus(A_SLUG_MOVE) )
		setAction(A_SLUG_MOVE);
}


void CPoisonSlug::getTouchedBy(CSpriteObject &theObject)
{
	if(mIsDead || theObject.mIsDead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
        mHealthPoints--;
        theObject.mIsDead = true;

        if(mHealthPoints == 0)
        {
            setAction( rand()%2 ? A_SLUG_STUNNED : A_SLUG_STUNNED_ALT );
            mIsDead = true;
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


int CPoisonSlug::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CPoisonSlug::process()
{
	performCollisions();
	
	performGravityMid();			

    if(!processActionRoutine())
        exists = false;

	
    (this->*mp_processState)();

    if( blockedl )
        xDirection = RIGHT;
    else if(blockedr)
        xDirection = LEFT;

}

}
