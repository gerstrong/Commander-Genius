/*
 * CInchWorm.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: gerstrong
 */

#include "CInchWorm.h"
#include "../../common/ai/CPlayerBase.h"




namespace galaxy {

CInchWorm::CInchWorm(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pMap, foeID, x, y, 0),
mInchWormContacts(0)
{
	setupGalaxyObjectOnMap(0x2E9A, 0);
	xDirection = LEFT;

    //performCollisions();
	
	processMove(0,-(1<<CSF));
	processMove(0,1<<CSF);
}


//const int CSF_MIN_DISTANCE_TO_FOLLOW = 8<<CSF;
const int CSF_DISTANCE_TO_FOLLOW_TOLERANCE = 1<<CSF;
//const int INCHWORM_SPEED = 2;


bool CInchWorm::isNearby(CSpriteObject &theObject)
{
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		mInchWormContacts = 0;
		const int dx = player->getXMidPos() - getXMidPos();

		if( dx<-CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
		{
			xDirection = LEFT;
		}
		else if( dx>+CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
		{
			xDirection = RIGHT;
		}
	}
    else if( CInchWorm *inchworm = dynamic_cast<CInchWorm*>(&theObject) )
	{
        const auto delta = inchworm->getMidPos() - getMidPos();

        int absdx = (delta.x<0) ? -delta.x : delta.x;
        int absdy = (delta.y<0) ? -delta.y : delta.y;

		if( absdx < CSF_DISTANCE_TO_FOLLOW_TOLERANCE &&
		    absdy < CSF_DISTANCE_TO_FOLLOW_TOLERANCE )
		{
			mInchWormContacts++;
		}


		if(mInchWormContacts >= 11)
		{
            if(!gEventManager.find<EventSpawnFoot>())
            {
                gEventManager.add( new EventSpawnFoot(  getXMidPos(), getYUpPos(), 67 ) );
            }
        }
    }

	return true;
}


int CInchWorm::checkSolidD( int x1, int x2, int y2, const bool push_mode )
{
	turnAroundOnCliff( x1, x2, y2 );

	return CGalaxySpriteObject::checkSolidD(x1, x2, y2, push_mode);
}


void CInchWorm::process()
{
    mInchWormContacts = 0;

	performCollisions();

	performGravityLow();

	if(!processActionRoutine())
		exists = false;

    if( blockedl && !onslope)
    {
        xDirection = RIGHT;
    }
    else if(blockedr && !onslope)
    {
        xDirection = LEFT;
    }

	if(!getActionStatus(0))
		return;

	// Move normally in the direction

    //const auto vel = (m_Action.h_anim_move>>3)*((rand()%2)+1);
    const auto vel = 10;

    if( xDirection == RIGHT )
    {
        moveRight( vel );
    }
	else
    {
        moveLeft( vel );
    }

}


} /* namespace galaxy */
