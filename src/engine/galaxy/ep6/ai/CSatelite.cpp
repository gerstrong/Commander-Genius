#include "CSatelite.h"
#include <engine/galaxy/common/ai/CPlayerBase.h>

/*
$1C8EW  #Sateleite
$1CACW  #Sateleite
$1CCAW  #Sateleite
$1CE8W  #Sateleite 
*/

namespace galaxy
{
    

const int MOVE_SPEED = 40;


CSatelite::CSatelite(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CGalaxySpriteObject(pmap, foeID, x, y),
CMoveTarget(m_Pos, xDirection, yDirection)
{
	solid = false;
		
	setupGalaxyObjectOnMap(0x1C8E, 0);
	
	fetchInitialDir(xDirection, yDirection, *mp_Map);
	detectNextTarget(target, xDirection, yDirection);
}


void CSatelite::processFlying()
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
	
	// Happens when the rocket find a place where to stop
	/*if(object == 0x6A || object == 0x69)
	{	    
	    VectorD2<int> newPlayerPos = m_Pos;
	    
	    newPlayerPos.y = getYDownPos()+(1<<CSF);
	    
	    mpCarriedPlayer->moveToForce(newPlayerPos);
	    mpCarriedPlayer->solid = true;
	    mpCarriedPlayer->dontdraw = false;
	    mpCarriedPlayer = nullptr;
	}*/
	
	if(object == 0x59)
	{
	    xDirection = LEFT;
	    yDirection = CENTER;
	}
	else
	{
	    readDirection(object, xDirection, yDirection );
	}
	
	// If there is an object that changes the direction of the plat, apply it!
	detectNextTarget(target, xDirection, yDirection);
    }
    
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

void CSatelite::getTouchedBy(CSpriteObject& theObject)
{
	/*if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		setAction(A_ROCKET_FLY);
	}*/
}

void CSatelite::process()
{
	if(!processActionRoutine())
	    exists = false;
	
	processFlying();
}    
    
    
};