/*
 * CSkypest.cpp
 *
 *  Created on: 21.08.2011
 *      Author: gerstrong
 */

#include "CSkypest.h"
#include "engine/galaxy/ai/CPlayerLevel.h"

namespace galaxy {

#define A_SKYPEST_FLY		0
#define A_SKYPEST_SQUISHED	2
#define A_SKYPEST_LICKEYES	3

const int FLY_SPEED = 50;

CSkypest::CSkypest(CMap *pmap, Uint32 x, Uint32 y) :
CObject(pmap, x, y, OBJ_NONE)
{
	setupGalaxyObjectOnMap(0x2486, A_SKYPEST_FLY);
	mp_processState = &CSkypest::processFly;
	m_hDir = RIGHT;
	m_vDir = UP;
}

bool CSkypest::isNearby(CObject &theObject)
{
	if(getActionStatus(A_SKYPEST_LICKEYES))
	{
		if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
		{
			const int dx = player->getXMidPos() - getXMidPos();

			if( dx<0 )
				m_hDir = LEFT;
			else
				m_hDir = RIGHT;
		}
	}
	return true;
}

void CSkypest::getTouchedBy(CObject &theObject)
{
	if(dead || theObject.dead)
		return;

	// When Keen touches the Council Member exit the level and add one to the council list
	if( CPlayerLevel *Player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		// Check if player is using pogo and Skypest is licking
		if( getActionStatus(A_SKYPEST_LICKEYES) || Player->isPogoing() )
		{
			dead = true;
			playSound(SOUND_SQUISH_SKYPEST);
			setAction(A_SKYPEST_SQUISHED);
		}
		else
		{
			// else Keen will die
			Player->kill();
		}
	}
}

void CSkypest::processOnFloor()
{
	if(getActionStatus(A_SKYPEST_FLY))
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
		m_vDir = UP;
		mp_processState = &CSkypest::processOnFloor;
		setAction(A_SKYPEST_LICKEYES);
	}
	else if(blockedu)
	{
		m_vDir = DOWN;
	}

	if(blockedl)
	{
		m_hDir = RIGHT;
	}
	else if(blockedr)
	{
		m_hDir = LEFT;
	}

	// When someting is blocking change the direction
	if(m_vDir == UP)
		moveUp(FLY_SPEED);
	else if(m_vDir == DOWN)
		moveDown(FLY_SPEED);

	if(m_hDir == LEFT)
		moveLeft(FLY_SPEED);
	else if(m_hDir == RIGHT)
		moveRight(FLY_SPEED);
}

void CSkypest::process()
{
	processActionRoutine();

	(this->*mp_processState)();
}

} /* namespace galaxy */
