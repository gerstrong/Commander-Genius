/*
 * CEnemyShot.cpp
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 */

#include "CEnemyShot.h"
#include "CPlayerBase.h"

namespace galaxy {



CEnemyShot::CEnemyShot(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y,
        const int actionFormatOffset, const int xDir, const int yDir,
                       const int speed, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
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
    moveXDir(m_speed*xDirection);    
    moveYDir(m_speed*yDirection);
    
    // If it collides against something, make it non-existent
    if( (blockedd && yDirection == DOWN) || 
	(blockedl && xDirection == LEFT) ||
	(blockedu && yDirection == UP) || 
	(blockedr && xDirection == RIGHT) )
    {
	exists = false;
    }
    
    if(!processActionRoutine())
	exists = false;    
}

} /* namespace galaxy */
