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
		const direction_t vertdir, const int actionOffset) :
CGalaxySpriteObject(pmap, foeID, x, y),
CPlatform(pmap, foeID, x, y),
targetmode(false)
{

	m_ActionBaseOffset = actionOffset;

	setActionForce(A_PLATFORM_MOVE);
	setActionSprite();

	xDirection = 0;
	yDirection = vertdir;

	const Uint32 xPos = getXMidPos();
	const Uint32 yPos = getYMidPos();
	target.x = xPos;
	target.y = yPos;

	calcBoundingBoxes();
}


void CVarPlatform::process()
{

	const Uint32 xPos = getXMidPos()-(1<<(CSF-1));
	const Uint32 yPos = getYMidPos();

	const Uint16 object = mp_Map->getPlaneDataAt(2, xPos, yPos);


	int xBlockPos = (target.x>>CSF);
	int yBlockPos = (target.y>>CSF);

	xBlockPos = (((xBlockPos<<1) + 1)<<(CSF-1));
	yBlockPos = (((yBlockPos<<1) + 1)<<(CSF-1));

	xBlockPos = xBlockPos - xPos;
	yBlockPos = yBlockPos - yPos;


	const int xBlockPosAbs = (xBlockPos<0) ? -xBlockPos : xBlockPos;
	const int yBlockPosAbs = (yBlockPos<0) ? -yBlockPos : yBlockPos;


	if(!targetmode)
	{
		if( xBlockPosAbs >= (1<<CSF) || yBlockPosAbs >= (1<<CSF) )
		{
			if( object>90 && object<99 )
			{
				targetmode = true;
				target.x = xPos;
				target.y = yPos;
			}
		}
	}
	else
	{

		if( xBlockPosAbs < MOVE_SPEED && yBlockPosAbs < MOVE_SPEED )
		{
			targetmode = false;

			// If there is an object that changes the direction, do it!
			switch( object )
			{
			case 91:
				xDirection = 0;
				yDirection = UP;
				break;
			case 92:
				xDirection = RIGHT;
				yDirection = 0;
				break;
			case 93:
				xDirection = 0;
				yDirection = DOWN;
				break;
			case 94:
				xDirection = LEFT;
				yDirection = 0;
				break;
			case 95:
				xDirection = RIGHT;
				yDirection = UP;
				break;
			case 96:
				xDirection = RIGHT;
				yDirection = DOWN;
				break;
			case 97:
				xDirection = LEFT;
				yDirection = DOWN;
				break;
			case 98:
				xDirection = LEFT;
				yDirection = UP;
				break;
			default:
				break;
			}

		}
		else
		{

			xDirection = 0;
			if(xBlockPosAbs >= MOVE_SPEED)
			{

				if( xBlockPos < 0 )
					xDirection = LEFT;
				else if( xBlockPos > 0 )
					xDirection = RIGHT;
			}

			yDirection = 0;
			if(yBlockPosAbs >= MOVE_SPEED)
			{

				if( yBlockPos < 0 )
					yDirection = UP;
				else if( yBlockPos > 0 )
					yDirection = DOWN;
			}

		}

	}


	if(yDirection == UP && blockedu)
		yDirection = DOWN;
	else if(yDirection == DOWN && blockedd)
		yDirection = UP;

	// direction change is processed here!
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

