/*
 * CVolteface.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CVolteface.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$1CCCW #Volte Face
$1CEAW #Volte Face
$1D08W #Volte Face
$1D26W #Volte Face
$1D44W #Volte Face stunned
 */


namespace galaxy {  
  
enum SPARKYACTIONS
{
A_VOLTFACE_MOVE = 0,
A_VOLTFACE_STUNNED = 4
};

const int MOVE_SPEED = 50;

  
CVolteface::CVolteface(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y),
mTimer(0),
targetmode(false)
{
	mActionMap[A_VOLTFACE_MOVE] = (void (CStunnable::*)()) &CVolteface::processMoving;
	mActionMap[A_VOLTFACE_STUNNED] = (void (CStunnable::*)()) &CVolteface::processStunned;
	
	m_ActionBaseOffset = 0x1CCC;

	setActionForce(A_VOLTFACE_MOVE);
	setActionSprite();

	calcBoundingBoxes();
		
	fetchInitialDir();
	detectNextTarget(getPosition());	
}


void CVolteface::fetchInitialDir()
{
  for (int y = -1 ; y<=1 ; y++ )
  {
    for (int x = -1 ; x<=1 ; x++ )
    {
      if(x == 0 && y == 0)
	continue;
      
      const Uint16 object = mp_Map->getPlaneDataAt(2, getXPosition()+(x<<CSF), getYPosition()+(y<<CSF));
      
      readDirection(object, xDirection, yDirection);
      
      // Now check all eight cases and test if it matches
      if( x == xDirection && y == yDirection )
	return;
    }
  }
}


void CVolteface::readDirection(const Uint16 object, 	
				int &xDirection,		
				int &yDirection )
{
	switch( object )
	{
	    case 0x50:
	    case 0x5B:
	    case 0x24:
		xDirection = 0;
		yDirection = UP;
		break;
	    case 0x51:
	    case 0x5C:
	    case 0x25:
		xDirection = RIGHT;
		yDirection = 0;
		break;
	    case 0x52:
	    case 0x5d:
	    case 0x26:
		xDirection = 0;
		yDirection = DOWN;
		break;
	    case 0x53:
	    case 0x5E:
	    case 0x27:
		xDirection = LEFT;
		yDirection = 0;
		break;
	    case 0x5F:
		xDirection = RIGHT;
		yDirection = UP;
		break;
	    case 0x60:
		xDirection = RIGHT;
		yDirection = DOWN;
		break;
	    case 0x61:
		xDirection = LEFT;
		yDirection = DOWN;
		break;
	    case 0x62:
		xDirection = LEFT;
		yDirection = UP;
		break;
	    default:
		break;
	}
  
}


void CVolteface::processMoving()
{
    int xBlockPos = target.x - getXPosition();
    int yBlockPos = target.y - getYPosition();
    
    const int xBlockPosAbs = (xBlockPos<0) ? -xBlockPos : xBlockPos;
    const int yBlockPosAbs = (yBlockPos<0) ? -yBlockPos : yBlockPos;
    
    if( xBlockPosAbs < MOVE_SPEED && yBlockPosAbs < MOVE_SPEED )
    {
	const Uint16 object = mp_Map->getPlaneDataAt(2, target.x, target.y);
	
	VectorD2<int> speed(xBlockPos, yBlockPos);	    
	moveDir(speed);
	
	targetmode = false;
	
	// If there is an object that changes the direction of the plat, apply it!
	// TODO: Create a class for VarPlatforms and this foe which are common elements that are handled
	readDirection(object, xDirection, yDirection );
	detectNextTarget(target);
    }
    
    if(yDirection == UP && blockedu)
	yDirection = DOWN;
    else if(yDirection == DOWN && blockedd)
	yDirection = UP;
    
    if(xDirection == RIGHT && blockedr)
	xDirection = LEFT;
    else if(xDirection == LEFT && blockedl)
	xDirection = RIGHT;
    
    VectorD2<int> speed;
    
    if(yDirection == UP)
    {
	speed.y = -MOVE_SPEED;
    }
    else if(yDirection == DOWN)
    {
	speed.y = MOVE_SPEED;
    }    
    
    if(xDirection == RIGHT)
    {
	speed.x = MOVE_SPEED;
    }
    else if(xDirection == LEFT)
    {
	speed.x = -MOVE_SPEED;
    }
    
    moveDir(speed);
}

void CVolteface::processStunned()
{
  if(getActionStatus(A_VOLTFACE_MOVE))
  {  
    setAction(A_VOLTFACE_MOVE);
  }
}



void CVolteface::getTouchedBy(CSpriteObject &theObject)
{
	if( getActionNumber(A_VOLTFACE_STUNNED) )
	  return;
  
	if(theObject.dead)
	  return;		

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
	  setAction(A_VOLTFACE_STUNNED);
	  theObject.dead = true;
	}

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
	  player->kill();
	}
}



void CVolteface::detectNextTarget(const VectorD2<int> &oldTarget)
{   
    VectorD2<int> potTarget(oldTarget);
    
    potTarget = (potTarget>>CSF);
        
    if(yDirection == UP)
	potTarget.y--;
    else if(yDirection == DOWN)
	potTarget.y++;
    
    if(xDirection == LEFT)
	potTarget.x--;
    else if(xDirection == RIGHT)
	potTarget.x++;
    
    target = (potTarget<<CSF);    
}



void CVolteface::process()
{
	performCollisions();
	
	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
