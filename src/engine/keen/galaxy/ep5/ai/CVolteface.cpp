/*
 * CVolteface.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CVolteface.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include <base/interface/misc.h>

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

constexpr int MOVE_SPEED = 50;

CVolteface::CVolteface(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y),
CMoveTarget(m_Pos, xDirection, yDirection)
{
	mActionMap[A_VOLTFACE_MOVE] = (GASOFctr) &CVolteface::processMoving;
	mActionMap[A_VOLTFACE_STUNNED] = (GASOFctr) &CVolteface::processStunned;
	
	m_ActionBaseOffset = 0x1CCC;

	setActionForce(A_VOLTFACE_MOVE);
	setActionSprite();

	calcBoundingBoxes();		

	fetchInitialDir(xDirection, yDirection, *mpMap);
	solid = false;
}


void CVolteface::processMoving()
{
    int xBlockPos = target.x - getXPosition();
    int yBlockPos = target.y - getYPosition();
    
    const int xBlockPosAbs = (xBlockPos<0) ? -xBlockPos : xBlockPos;
    const int yBlockPosAbs = (yBlockPos<0) ? -yBlockPos : yBlockPos;
    
    if( xBlockPosAbs < MOVE_SPEED && yBlockPosAbs < MOVE_SPEED )
    {
        const Uint16 object = mpMap->getPlaneDataAt(2, target.x, target.y);

        GsVec2D<int> speed(xBlockPos, yBlockPos);
        moveDir(speed);

        // If there is an object that changes the direction of the plat, apply it!
        readDirection(object, xDirection, yDirection );
        detectNextTarget(target, xDirection, yDirection);
    }
    
    GsVec2D<int> speed;
    
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
  
	if(theObject.mIsDead)
	  return;		

	CStunnable::getTouchedBy(theObject);

	// Was it a bullet? Than make it stunned.
	if( dynamic_cast<CBullet*>(&theObject) )
	{
	  setAction(A_VOLTFACE_STUNNED);
	  theObject.mIsDead = true;
	}

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
      player->kill(false);
	}
}




void CVolteface::process()
{
	performCollisions();
	
	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();
}

}
