/*
 * CEnemyShot.cpp
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 */

#include "CEnemyShot.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"

namespace galaxy {



CEnemyShot::CEnemyShot(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
		const int actionFormatOffset, const int xDir, const int yDir, const int speed) :
CGalaxySpriteObject(pmap, foeID, x, y),
m_speed(speed)
{
	setupGalaxyObjectOnMap(actionFormatOffset, 0);

	xDirection = xDir;
	yDirection = yDir;

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
	if(xDirection == LEFT)
		moveLeft(m_speed);
	else if(xDirection == RIGHT)
		moveRight(m_speed);

	if(yDirection == UP)
		moveUp(m_speed);
	else if(yDirection == DOWN)
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
