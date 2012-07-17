/*
 * CEnemyShot.cpp
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 */

#include "CEnemyShot.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy {



CEnemyShot::CEnemyShot(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
		const int actionFormatOffset, const direction_t dir, const int speed) :
CGalaxySpriteObject(pmap, foeID, x, y),
m_speed(speed)
{
	setupGalaxyObjectOnMap(actionFormatOffset, 0);

	m_hDir = m_vDir = NONE;

	// Translate into two variable horizontal and vertical style
	if(dir == RIGHT || dir == LEFT)
		m_hDir = dir;
	else
		m_vDir = dir;

}

void CEnemyShot::getTouchedBy(CSpriteObject &theObject)
{
	if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
	{
		Player->kill();
	}
}


void CEnemyShot::process()
{
	if(m_hDir == LEFT)
		moveLeft(m_speed);
	else if(m_hDir == RIGHT)
		moveRight(m_speed);

	if(m_vDir == UP)
		moveUp(m_speed);
	else if(m_vDir == DOWN)
		moveDown(m_speed);

	// If it collides against something, make it non-existent
	if( blockedd || blockedl ||
		blockedu || blockedr )
	{
		exists = false;
	}

	if(!processActionRoutine())
			exists = false;

}

} /* namespace galaxy */
