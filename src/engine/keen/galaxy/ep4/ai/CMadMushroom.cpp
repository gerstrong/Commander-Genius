/*
 * CMadMushroom.cpp
 *
 *  Created on: 12.07.2011
 *      Author: gerstrong
 */

#include "CMadMushroom.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include "fileio/KeenFiles.h"

int bounceAmount;

namespace galaxy
{

const int A_MUSHROOM_BOUNCE = 0;

const int MUSHROOM_LOW_INERTIA = 90;
const int MUSHROOM_HIGH_INERTIA = 148;

CMadMushroom::CMadMushroom(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0),
jumpcounter(0)
{
	setupGalaxyObjectOnMap(0x20E4, A_MUSHROOM_BOUNCE);
	processActionRoutine();
	honorPriority = false;
	
	bounceAmount = 0;

    byte *ptr = gKeenFiles.exeFile.getRawData();
	ptr += 0xFF90;
	memcpy(&bounceAmount, ptr, 1 );
}


bool CMadMushroom::isNearby(CSpriteObject &theObject)
{
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			xDirection = LEFT;
		else
			xDirection = RIGHT;
	}

	return true;
}



void CMadMushroom::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.dead )
		return;

	if( CBullet *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		bullet->dead = true;
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}



void CMadMushroom::process()
{

	performGravityMid();
	performCollisions();

	if(!blockedd)
	{
		m_jumped = false;
	}

	if(blockedd && !m_jumped)
	{
		yinertia = -MUSHROOM_LOW_INERTIA;
		jumpcounter++;

		if( jumpcounter>=bounceAmount )
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

	if(!processActionRoutine())
			exists = false;
}

} /* namespace galaxy */
