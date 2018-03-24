/*
 * CSchoolFish.cpp
 *
 *  Created on: 01.08.2011
 *      Author: gerstrong
 */

#include "CSchoolFish.h"
#include "../../common/ai/CPlayerBase.h"

namespace galaxy {

#define A_DOPEFISHFOOD_NORMAL	0

const int CSF_MIN_DISTANCE_TO_FOLLOW = 8<<CSF;
const int CSF_MAX_DIST_TO_FOLLOW = 3<<CSF;
const int CSF_TIGHT_DIST_TO_FOLLOW = 1<<CSF;
const int FISH_SPEED = 40;

CSchoolFish::CSchoolFish(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0),
m_moveSpeed(0)
{
	setupGalaxyObjectOnMap(0x3692, A_DOPEFISHFOOD_NORMAL);
	xDirection = RIGHT;
	yDirection = DOWN;
}

bool CSchoolFish::isNearby(CSpriteObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		const int dx = player->getXMidPos() - getXMidPos();
		const int dy = player->getYMidPos() - getYMidPos();

        if( dx<-CSF_MAX_DIST_TO_FOLLOW )
		{
			xDirection = LEFT;
		}
        else if( dx>+CSF_MAX_DIST_TO_FOLLOW )
		{
			xDirection = RIGHT;
		}

        if( dy<-CSF_MAX_DIST_TO_FOLLOW )
		{
            if( dy<-CSF_TIGHT_DIST_TO_FOLLOW )
                yDirection = UP;
            else
                yDirection = (rand()%10) ? UP : DOWN;
		}
        else if( dy>+CSF_MAX_DIST_TO_FOLLOW )
		{
            if( dy>+CSF_TIGHT_DIST_TO_FOLLOW )
                yDirection = DOWN;
            else
                yDirection = (rand()%4) ? DOWN : UP;
		}

		int absdx = (dx<0) ? -dx : dx;
		int absdy = (dy<0) ? -dy : dy;

		if( absdx < CSF_MIN_DISTANCE_TO_FOLLOW &&
		    absdy < CSF_MIN_DISTANCE_TO_FOLLOW )
		{
			m_moveSpeed = FISH_SPEED*(rand()%2+1);
		}

	}

	return true;
}


void CSchoolFish::process()
{
	// Only move the fish, when it gets speed, which only happens, when Keen is nearby them
	if(m_moveSpeed)
	{
		if(xDirection == LEFT)
			moveLeft(m_moveSpeed);
		else if(xDirection == RIGHT)
			moveRight(m_moveSpeed);

		if(yDirection == UP)
			moveUp(m_moveSpeed);
		else if(yDirection == DOWN)
			moveDown(m_moveSpeed);
	}

    // Blocking should make these fishs change the direction
    if( blockedl )
    {
        xDirection = RIGHT;
    }
    else if(blockedr)
    {
        xDirection = LEFT;
    }

    if( blockedd )
    {
        yDirection = UP;
    }
    else if(blockedu)
    {
        yDirection = DOWN;
    }

	processActionRoutine();
}

} /* namespace galaxy */
