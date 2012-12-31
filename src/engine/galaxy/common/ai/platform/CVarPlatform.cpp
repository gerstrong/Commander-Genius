/*
 * CVarPlatform.cpp
 *
 *  Created on: Sep 6, 2012
 *      Author: gerstrong
 */

#include "CVarPlatform.h"

namespace galaxy {


const int MOVE_SPEED = 20;

CVarPlatform::CVarPlatform(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y,
		const direction_t horidir, 
		const direction_t vertdir,
		const int actionOffset) :
CGalaxySpriteObject(pmap, foeID, x, y),
CPlatform(pmap, foeID, x, y),
targetmode(false)
{
	m_ActionBaseOffset = actionOffset;

	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();

	xDirection = horidir;
	yDirection = vertdir;

	calcBoundingBoxes();
	
	detectNextTarget(getPosition());	
}


void CVarPlatform::detectNextTarget(const VectorD2<int> &oldTarget)
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

void CVarPlatform::process()
{
    int xBlockPos = target.x - getXPosition();
    int yBlockPos = target.y - getYPosition();
    
    const int xBlockPosAbs = (xBlockPos<0) ? -xBlockPos : xBlockPos;
    const int yBlockPosAbs = (yBlockPos<0) ? -yBlockPos : yBlockPos;
    
    if( xBlockPosAbs < MOVE_SPEED && yBlockPosAbs < MOVE_SPEED )
    {
	const Uint16 object = mp_Map->getPlaneDataAt(2, target.x, target.y);
	
	VectorD2<int> speed(xBlockPos, yBlockPos);	    
	movePlat(speed);
	
	targetmode = false;
	
	// If there is an object that changes the direction of the plat, apply it!
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
		xDirection = 0;
		yDirection = 0;
		
		break;
	}
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
    
    movePlat(speed);
    
    CPlatform::process();
}

}

