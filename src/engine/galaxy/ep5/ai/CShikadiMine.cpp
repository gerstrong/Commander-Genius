/*
 * CShikadiMine.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CShikadiMine.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$2608W #Shikadi mine sit 0
$2626W #Shikadi mine move 1
$2644W #Shikadi mine change direction 2
$2662W #Shikadi mine detonate 3
$2680W #Shikadi mine detonate
$269EW #Shikadi mine detonate
$26BCW #Shikadi mine detonate
$26DAW #Shikadi mine detonate
$26F8W #Shikadi mine detonate
$2716W #Shikadi mine fragments -> TODO: This is another object! We should implement a fragments class somehow.
 */

// TODO: Overload the draw function, because the mine has an eye which moves around in the sprite

namespace galaxy {  
  
enum MINEACTIONS
{
A_MINE_SIT = 0,
A_MINE_MOVE = 1,
A_MINE_CHANGE_DIR = 2,
A_MINE_DETONATE = 3
};


const int TIME_SIT = 150;
const int TIME_MOVE = 150;
const int TIME_CHANGE_DIR = 150;

const int MOVE_SPEED = 10;
  
CShikadiMine::CShikadiMine(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0)
{
    mActionMap[A_MINE_SIT] = (void (CStunnable::*)()) &CShikadiMine::processSit;
    mActionMap[A_MINE_MOVE] = (void (CStunnable::*)()) &CShikadiMine::processMoving;
    mActionMap[A_MINE_CHANGE_DIR] = (void (CStunnable::*)()) &CShikadiMine::processChangeDir;
    mActionMap[A_MINE_DETONATE] = (void (CStunnable::*)()) &CShikadiMine::processDetonate;
  
    // Adapt this AI
    setupGalaxyObjectOnMap(0x2608, A_MINE_SIT);
	
    xDirection = LEFT;
    yDirection = UP;
}


void CShikadiMine::processSit()
{
  mTimer++;
  if( mTimer < TIME_SIT )
      return;
  
  mTimer = 0;
  
  setAction(A_MINE_MOVE);
}


void CShikadiMine::processMoving()
{          
    
  // Move normally in the direction
  moveXDir( xDirection*MOVE_SPEED );
  moveYDir( yDirection*MOVE_SPEED );
  
  mTimer++;
  if( mTimer < TIME_MOVE )
      return;
  
  mTimer = 0;
  
  //setAction(A_MINE_CHANGE_DIR);
}

void CShikadiMine::processChangeDir()
{
  // Look at the Player coords and define a direction
  xDirection = yDirection = CENTER;
  if(getProbability(500))
      xDirection = mKeenAlignmentX;
  else
      yDirection = mKeenAlignmentY;
  
  mTimer++;
  if( mTimer < TIME_CHANGE_DIR )
      return;  
  
  mTimer = 0;
  
    if( blockedl && xDirection == LEFT )
    {
	xDirection = CENTER;
	yDirection = mKeenAlignmentY;
	
	if(yDirection == UP && blockedu)
	    yDirection = DOWN;
	    
	if(yDirection == DOWN && blockedd)
	{
	    yDirection = CENTER;
	    xDirection = RIGHT;
	}
    }
    else if( blockedr && xDirection == RIGHT )
    {
	xDirection = CENTER;
	yDirection = mKeenAlignmentY;
	
	if(yDirection == DOWN  && blockedd)
	    yDirection = UP;
	    
	if(yDirection == UP && blockedu)
	{
	    yDirection = CENTER;
	    xDirection = LEFT;
	}	
    }    
    
    if( blockedu && yDirection == UP )
    {
	yDirection = CENTER;
	xDirection = mKeenAlignmentX;
	
	if(xDirection == LEFT && blockedl)
	    xDirection = RIGHT;
	    
	if(xDirection == RIGHT && blockedr)
	{
	    xDirection = CENTER;
	    yDirection = DOWN;
	}
    }
    else if( blockedd && yDirection == DOWN )
    {
	yDirection = CENTER;
	xDirection = mKeenAlignmentX;
	
	if(xDirection == RIGHT && blockedr)
	    xDirection = LEFT;
	    
	if(xDirection == LEFT && blockedl)
	{
	    xDirection = CENTER;
	    yDirection = UP;
	}	
    }    
  
  setAction(A_MINE_MOVE);  
}

void CShikadiMine::processDetonate()
{  
  if(!processActionRoutine())
  {
      dead = true;
      exists = false;
      
      // TODO: Spawn little explosion shards here!
  }  
}

bool CShikadiMine::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{	    	    
		if( player->getXMidPos() < getXMidPos() )
			mKeenAlignmentX = LEFT;
		else
			mKeenAlignmentX = RIGHT;

		if( player->getYMidPos() < getYMidPos() )
			mKeenAlignmentY = UP;
		else
			mKeenAlignmentY = DOWN;
	}

	return true;
}

void CShikadiMine::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	if(getActionNumber(A_MINE_DETONATE))
		return;

	
	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
	    theObject.dead = true;
	    playSound(SOUND_MINEEXPLODE);
	    setAction(A_MINE_DETONATE);
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
	    playSound(SOUND_MINEEXPLODE);
	    setAction(A_MINE_DETONATE);
	    player->kill();
	}
}


void CShikadiMine::process()
{
	performCollisions();
	
	//if(!processActionRoutine())
	//    exists = false;
	
	(this->*mp_processState)();
}



///////////////////
/// Mine shards ///
///////////////////

CMineShards::CMineShards(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xSpeed) :
CStunnable(pmap, foeID, x, y),
mXSpeed(xSpeed)
{
  xDirection = (xSpeed < 0) ? LEFT : RIGHT;
}


void CMineShards::getTouchedBy(CSpriteObject& theObject)
{
	if(dead || theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}		
}




void CMineShards::process()
{
	performCollisions();
	
	performGravityMid();

	if( blockedd )
	{
	  dead = true;
	  return;
	}
	
	// TODO: Special case when a shard touches the Omegamatic Core
	
	moveXDir(mXSpeed);
	
}


}
