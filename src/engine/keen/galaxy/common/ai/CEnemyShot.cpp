/*
 * CEnemyShot.cpp
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 */

#include "CEnemyShot.h"
#include "CPlayerBase.h"

namespace galaxy {



CEnemyShot::CEnemyShot(std::shared_ptr<CMap> pMap,
                       const int foeID,
                       const int x, const int y,
                       const int actionFormatOffset,
                       const int xDir, const int yDir,
                       const float speed, const int sprVar,
                       const bool climbOnPoles) :
CGalaxySpriteObject(pMap, foeID, x, y, sprVar),
m_speedF(speed),
mClimbOnPoles(climbOnPoles)
{
	setupGalaxyObjectOnMap(actionFormatOffset, 0);

	xDirection = xDir;
	yDirection = yDir;

}

void CEnemyShot::getTouchedBy(CSpriteObject &theObject)
{
	if(CPlayerBase *Player = dynamic_cast<CPlayerBase*>(&theObject))
	{
        Player->kill(false);
	}
}


void CEnemyShot::process()
{    
    m_speedInt += m_speedF;

    if(m_speedInt >= 1.0)
    {
        const int speed = m_speedInt;

        moveXDir(speed*xDirection);
        moveYDir(speed*yDirection);

        m_speedInt -= float(speed);
    }        

    // Shots climbing up poles must be in contact with poles
    if(mClimbOnPoles)
    {
        if(!hitdetectWithTileProperty(1, getXMidPos(), getYMidPos()))
        {
            exists = false;
        }
    }
    
    // If it collides against something, make it non-existent
    if( (blockedd && yDirection == DOWN) ||
            (blockedl && xDirection == LEFT) ||
            (blockedu && yDirection == UP) ||
            (blockedr && xDirection == RIGHT) ||
            onslope )
    {
        exists = false;
    }
    
    if(!processActionRoutine())
    {
        exists = false;
    }
}

} /* namespace galaxy */
