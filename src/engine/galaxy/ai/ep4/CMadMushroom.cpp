/*
 * CMadMushroom.cpp
 *
 *  Created on: 12.07.2011
 *      Author: gerstrong
 */

#include "CMadMushroom.h"
#include "engine/galaxy/ai/CPlayerLevel.h"

namespace galaxy
{

const int A_MUSHROOM_BOUNCE = 0;

const int MUSHROOM_LOW_INERTIA = 60;
const int MUSHROOM_HIGH_INERTIA = 150;

CMadMushroom::CMadMushroom(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
CStunnable(pmap, foeID, x, y),
jumpcounter(0)
{
	setupGalaxyObjectOnMap(0x20E4, A_MUSHROOM_BOUNCE);
}


bool CMadMushroom::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			m_hDir = LEFT;
		else
			m_hDir = RIGHT;
	}

	return true;
}



void CMadMushroom::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.dead)
		return;

	CStunnable::getTouchedBy(theObject);

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}



void CMadMushroom::process()
{

	processFalling();

	performCollisions();

	if(!blockedd)
	{
		m_jumped = false;
	}

	if(blockedd && !m_jumped)
	{
		yinertia = -MUSHROOM_LOW_INERTIA;
		jumpcounter++;

		if( jumpcounter>=3 )
		{
			yinertia = -MUSHROOM_HIGH_INERTIA;
			jumpcounter = 0;
			playSound( SOUND_BOUNCE_HIGH );
		}
		else
		{
			playSound( SOUND_BOUNCE_LOW );
		}

		m_jumped = true;
	}

	processActionRoutine();
}

} /* namespace galaxy */
