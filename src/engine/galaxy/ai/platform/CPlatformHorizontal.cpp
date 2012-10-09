/*
 * CPlatformHorizontal.cpp
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#include "CPlatformHorizontal.h"
#include "common/CBehaviorEngine.h"

namespace galaxy {

CPlatformHorizontal::CPlatformHorizontal(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
CPlatform(pmap, foeID, x, y),
mp_BoostEngObjLeft(NULL),
mp_BoostEngObjRight(NULL)
{
	xDirection = RIGHT;
	yDirection = 0;

	const int episode = g_pBehaviorEngine->getEpisode();

	if(episode == 5)
	{
		m_ActionBaseOffset = 0x1B7C;
		setActionForce(A_PLATFORM_MOVE);
		setActionSprite();
		calcBoundingBoxes();
	}
	else
	{
		setActionForce(A_PLATFORM_MOVE);
		setActionSprite();
		calcBoundingBoxes();

		// Setup boost effects
		mp_BoostEngObjLeft = new CEngineParticleSprites(mp_Map, 0, x, y+(4<<STC), false, true);
		mp_BoostEngObjRight = new CEngineParticleSprites(mp_Map, 0, x+m_BBox.x2+(4<<STC), y+(4<<STC), false, false);

		g_pBehaviorEngine->m_EventList.add( new EventSpawnObject( mp_BoostEngObjLeft ) );
		g_pBehaviorEngine->m_EventList.add( new EventSpawnObject( mp_BoostEngObjRight ) );

		mp_BoostEngObjLeft->dontdraw = false;
		mp_BoostEngObjRight->dontdraw = true;
	}
}

const int MOVE_HORIZ_SPEED = 20;

void CPlatformHorizontal::process()
{
	const Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
	
	performCollisions();

	// If there is a blocker, change the direction
	if( object == 31 )
	{
		xDirection = (xDirection == RIGHT) ? LEFT : RIGHT;
	}
	
	if(blockedr)
	{
	    xDirection = LEFT;
	}
	else if(blockedl)
	{
	    xDirection = RIGHT;
	}

	// direction change is processed here!
	if(xDirection == RIGHT)
	{
		if(mp_BoostEngObjLeft)
		{
			mp_BoostEngObjLeft->dontdraw = true;
			mp_BoostEngObjRight->dontdraw = false;
		}
	}
	else if(xDirection == LEFT)
	{
		if(mp_BoostEngObjLeft)
		{
			mp_BoostEngObjLeft->dontdraw = false;
			mp_BoostEngObjRight->dontdraw = true;
		}
	}

	if(xDirection == RIGHT)
	{
		if(mp_BoostEngObjLeft)
		{
			mp_BoostEngObjLeft->moveRight(MOVE_HORIZ_SPEED);
			mp_BoostEngObjRight->moveRight(MOVE_HORIZ_SPEED);
		}
		movePlatRight(MOVE_HORIZ_SPEED);
	}
	else
	{
		if(mp_BoostEngObjLeft)
		{
			mp_BoostEngObjLeft->moveLeft(MOVE_HORIZ_SPEED);
			mp_BoostEngObjRight->moveLeft(MOVE_HORIZ_SPEED);
		}
		movePlatLeft(MOVE_HORIZ_SPEED);
	}

	CPlatform::process();
}

}
