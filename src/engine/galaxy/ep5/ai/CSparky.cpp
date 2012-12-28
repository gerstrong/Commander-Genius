/*
 * CSparky.cpp
 *
 *  Created on: 26 Dez 2012
 *      Author: Gerstrong
 */


#include "CSparky.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include "misc.h"

const int TIME_UNTIL_MOVE = 5;

namespace galaxy {  
  
enum SPARKYACTIONS
{
A_SPARKY_WALK = 0,	/* Ordinary slug_move action */
A_SPARKY_LOOK = 4,
A_SPARKY_PREPARE_CHARGE = 12,
A_SPARKY_CHARGE = 16,
A_SPARKY_TURN = 20,
A_SPARKY_STUNNED = 23
};
  
CSparky::CSparky(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
  	mActionMap[A_SPARKY_WALK] = (void (CStunnable::*)()) &CSparky::processWalking;
	mActionMap[A_SPARKY_STUNNED] = &CStunnable::processGettingStunned;
  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x1F0C, A_SPARKY_WALK);
	
	xDirection = LEFT;
}



void CSparky::processWalking()
{
  mTimer++;
  
  if(mTimer < TIME_UNTIL_MOVE)
    return;
  
  // Move normally in the direction
  if( xDirection == RIGHT )
  {
    //moveRight( m_Action.velX<<1 );
    moveRight( m_Action.velX );
  }
  else
  {
    //moveLeft( m_Action.velX<<1 );
    moveLeft( m_Action.velX );
  }
   
  mTimer = 0;
}


void CSparky::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
	  // TODO: Sparky stunned sound missing
		setAction(A_SPARKY_STUNNED);
		dead = true;
		theObject.dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


int CSparky::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CSparky::process()
{
	performCollisions();
	
	performGravityMid();

	if( blockedl )
		xDirection = RIGHT;
	else if(blockedr)
		xDirection = LEFT;

	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
