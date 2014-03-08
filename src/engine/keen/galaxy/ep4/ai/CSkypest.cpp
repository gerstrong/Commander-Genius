/*
 * CSkypest.cpp
 *
 *  Created on: 21.08.2011
 *      Author: gerstrong
 */

#include "CSkypest.h"
#include "../../common/ai/CPlayerLevel.h"

namespace galaxy {

#define A_SKYPEST_FLY		0
#define A_SKYPEST_SQUISHED	2
#define A_SKYPEST_LICKEYES	3

const int FLY_SPEED = 50;

CSkypest::CSkypest(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0)
{
	setupGalaxyObjectOnMap(0x2486, A_SKYPEST_FLY);
	mp_processState = &CSkypest::processFly;
	xDirection = RIGHT;
	yDirection = UP;
}

bool CSkypest::isNearby(CSpriteObject &theObject)
{
	if(getActionStatus(A_SKYPEST_LICKEYES))
	{
		if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
		{
			const int dx = player->getXMidPos() - getXMidPos();

			if( dx<0 )
				xDirection = LEFT;
			else
				xDirection = RIGHT;
		}
	}
	return true;
}

void CSkypest::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead )
		return;

	// When Keen touches the Council Member exit the level and add one to the council list
	if( CPlayerLevel *Player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		// Check if player is using pogo and Skypest is licking
		if( getActionNumber(A_SKYPEST_LICKEYES) && Player->isPogoing() )
		{
			dead = true;
			playSound(SOUND_SQUISH_SKYPEST);
			setAction(A_SKYPEST_SQUISHED);
		}
		else if( getActionNumber(A_SKYPEST_FLY) )
		{
			// else Keen will die
			Player->kill();
		}
	}
}

void CSkypest::processOnFloor()
{
	if(getActionStatus(A_SKYPEST_FLY)) // it is not set right, so we do it here!
	{
		mp_processState = &CSkypest::processFly;
		blockedu = blockedd = false;
		blockedl = blockedr = false;
		setAction(A_SKYPEST_FLY);
	}
}

void CSkypest::processFly()
{
	// Flying to the direction Code
	if(blockedd)
	{
		yDirection = UP;
		mp_processState = &CSkypest::processOnFloor;
		setAction(A_SKYPEST_LICKEYES);
	}
	else if(blockedu)
	{
		yDirection = DOWN;
	}

	if(blockedl)
	{
		xDirection = RIGHT;
	}
	else if(blockedr)
	{
		xDirection = LEFT;
	}

	// When someting is blocking change the direction
	if(yDirection == UP)
		moveUp(FLY_SPEED);
	else if(yDirection == DOWN)
		moveDown(FLY_SPEED);

	if(xDirection == LEFT)
		moveLeft(FLY_SPEED);
	else if(xDirection == RIGHT)
		moveRight(FLY_SPEED);
}

void CSkypest::process()
{
    performCollisions();
    
    (this->*mp_processState)();

    if(!processActionRoutine())
	exists = false;
}

} /* namespace galaxy */
