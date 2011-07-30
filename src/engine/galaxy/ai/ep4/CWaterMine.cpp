/*
 * CWaterMine.cpp
 *
 *  Created on: 30.07.2011
 *      Author: gerstrong
 */

#include "CWaterMine.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy {

#define A_MINE_MOVE			0
#define A_MINE_EXPLODE		1

const int MINE_SPEED = 10;

CWaterMine::CWaterMine(CMap *pmap, Uint32 x, Uint32 y, const bool vertical) :
CObject(pmap, x, y, OBJ_NONE)
{
	m_vDir = m_hDir = NONE;

	if(vertical)
		m_vDir = DOWN;
	else
		m_hDir = RIGHT;

	setupGalaxyObjectOnMap(0x3890, A_MINE_MOVE);
	mp_processState = &CWaterMine::processMove;
}

void CWaterMine::getTouchedBy(CObject &theObject)
{
	if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
	{
		Player->kill();
	}
}


void CWaterMine::processMove()
{
	if(m_vDir)
	{
		const Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());

		// If there is a blocker, change the direction
		if( object == 31 )
			m_vDir = (m_vDir == UP) ? DOWN : UP;


		if(blockedd)
			m_vDir = UP;
		else if(blockedu)
			m_vDir = DOWN;

		if(m_vDir == DOWN)
			moveDown(MINE_SPEED);
		else
			moveUp(MINE_SPEED);
	}
	else
	{
		const Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());

		// If there is a blocker, change the direction
		if( object == 31 )
			m_hDir = (m_hDir == RIGHT) ? LEFT : RIGHT;

		if(blockedl)
			m_hDir = RIGHT;
		else if(blockedr)
			m_hDir = LEFT;

		if(m_hDir == LEFT)
			moveLeft(MINE_SPEED);
		else
			moveRight(MINE_SPEED);
	}
}

void CWaterMine::processExplode()
{

}

void CWaterMine::process()
{
	(this->*mp_processState)();

	processActionRoutine();
}

} /* namespace galaxy */
