/*
 * CMimrock.cpp
 *
 *  Created on: 18.09.2011
 *      Author: gerstrong
 */

#include "CMimrock.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy {

enum MIMROCK_ACTION
{
A_MIMROCK_SIT = 0,
A_MIMROCK_WALK = 1,
A_MIMROCK_JUMP = 7,
A_MIMROCK_BOUNCE = 10,
A_MIMROCK_SHOT = 11,
A_MIMROCK_STUNNED = 12,
};

const int CSF_DISTANCE_TO_FOLLOW_TOLERANCE = 5<<CSF;
const int WALK_SPEED = 10;

CMimrock::CMimrock(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CStunnable(pmap, foeID, x, y)
{
	mActionMap[A_MIMROCK_SIT] = (void (CStunnable::*)()) &CMimrock::processSit;
	mActionMap[A_MIMROCK_WALK] = (void (CStunnable::*)()) &CMimrock::processWalk;
	mActionMap[A_MIMROCK_JUMP] = (void (CStunnable::*)()) &CMimrock::processJump;
	mActionMap[A_MIMROCK_BOUNCE] = (void (CStunnable::*)()) &CMimrock::processBounce;
	mActionMap[A_MIMROCK_STUNNED] = &CStunnable::processGettingStunned;

	setupGalaxyObjectOnMap(0x343A, A_MIMROCK_SIT);
	xDirection = RIGHT;
	honorPriority = false;
	
	processMove(0, -(1<<CSF));	
	processMove(0, 1<<CSF);	
}


void CMimrock::getTouchedBy(CSpriteObject &theObject)
{
    if(dead || theObject.dead)
	return;
    
    if( !getActionNumber(A_MIMROCK_SIT) )
    {    
	CStunnable::getTouchedBy(theObject);
	
	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
	    setAction( A_MIMROCK_STUNNED );
	    theObject.dead = true;
	}
    }
    
    if( getActionNumber(A_MIMROCK_WALK) || getActionNumber(A_MIMROCK_JUMP) || getActionNumber(A_MIMROCK_BOUNCE) )
    {
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
    }
}


bool CMimrock::isNearby(CSpriteObject &theObject)
{
    if(dead || theObject.dead)
	return true;
    
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
	const int dx = player->getXMidPos() - getXMidPos();
	
	if( dx>-CSF_DISTANCE_TO_FOLLOW_TOLERANCE &&
	    dx<+CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
	{
	    if( dx<0 )
		xDirection = LEFT;
	    else
		xDirection = RIGHT;
	    
	    setAction(A_MIMROCK_WALK);
	}
    }
    
    return true;
}

void CMimrock::processSit()
{
	// When sitting the rock doesn't do any thing, so this stays empty.
}

void CMimrock::processWalk()
{
	if(xDirection == LEFT)
	{
	    moveLeft(WALK_SPEED);
	}
	else
	{
	    moveRight(WALK_SPEED);
	}

	if(getActionStatus(A_MIMROCK_SIT))
	{
	    setAction(A_MIMROCK_SIT);
	}

}

void CMimrock::processJump()
{
	// TODO: Code here!
}

void CMimrock::processBounce()
{
	// TODO: Code here!
}

void CMimrock::process()
{    
    performCollisions();
    performGravityLow();
    
    if(dead)
	return;

    (this->*mp_processState)();
	
    processActionRoutine();	
    
    if(getActionStatus(A_MIMROCK_STUNNED))
	dead = true;
}


} /* namespace galaxy */
