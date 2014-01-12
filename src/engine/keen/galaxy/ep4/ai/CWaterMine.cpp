/*
 * CWaterMine.cpp
 *
 *  Created on: 30.07.2011
 *      Author: gerstrong
 */

#include "CWaterMine.h"
#include "../../common/ai/CPlayerBase.h"

namespace galaxy {

#define A_MINE_MOVE			0
#define A_MINE_EXPLODE		1

const int MINE_SPEED = 20;

CWaterMine::CWaterMine(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const direction_t xDir, const bool vertical) :
CGalaxySpriteObject(pmap, foeID, x, y, 0)
{
	yDirection = 0;
	xDirection = xDir;
	
	if(vertical)
		yDirection = DOWN;

	setupGalaxyObjectOnMap(0x3890, A_MINE_MOVE);
	mp_processState = &CWaterMine::processMove;
}

void CWaterMine::getTouchedBy(CSpriteObject &theObject)
{
	if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
	{
		void (CWaterMine::*ExplodeHandler)() = &CWaterMine::processExplode;
		Player->kill();

		if(mp_processState != ExplodeHandler)
		{
			mp_processState = ExplodeHandler;
			setAction(A_MINE_EXPLODE);
			playSound(SOUND_MINE_EXPLOSION);
		}
	}
}


void CWaterMine::processMove()
{
	performCollisions();

	if(yDirection)
	{
		const Uint16 blockerUp = mp_Map->getPlaneDataAt(2, getXMidPos(), getYUpPos());
		const Uint16 blockerDown = mp_Map->getPlaneDataAt(2, getXMidPos(), getYDownPos());

		// If there is a blocker, block the Watermine
		if( blockerUp == 31 )
			blockedu = true;
		if( blockerDown == 31 )
			blockedd = true;

		// If the mine is really blocked, change the direction
		if(blockedd)
			yDirection = UP;
		else if(blockedu)
			yDirection = DOWN;

		if(yDirection == DOWN)
			moveDown(MINE_SPEED);
		else
			moveUp(MINE_SPEED);
	}
	else
	{
		const Uint16 blockerLeft = mp_Map->getPlaneDataAt(2, getXLeftPos(), getYMidPos());
		const Uint16 blockerRight = mp_Map->getPlaneDataAt(2, getXRightPos(), getYMidPos());

		// If there is a blocker, block the Watermine
		if( blockerLeft == 31 )
			blockedl = true;
		if( blockerRight == 31 )
			blockedr = true;

		// If the mine is really blocked, change the direction
		if(blockedl)
			xDirection = RIGHT;
		else if(blockedr)
			xDirection = LEFT;

		if(xDirection == LEFT)
			moveLeft(MINE_SPEED);
		else
			moveRight(MINE_SPEED);
	}
}

void CWaterMine::processExplode()
{
	// This is intententially left empty. The explosion just happens without any extra interaction
	// because it is taken over be action script
}

void CWaterMine::process()
{
	(this->*mp_processState)();

	if(!processActionRoutine())
			exists = false;

}

} /* namespace galaxy */
