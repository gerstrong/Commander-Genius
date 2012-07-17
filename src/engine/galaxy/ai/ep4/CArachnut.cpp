/*
 * CArachnut.cpp
 *
 *  Created on: 17.07.2012
 *      Author: gerstong
 */

#include "CArachnut.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "engine/galaxy/ai/CBullet.h"
#include "misc.h"

namespace galaxy
{

const int A_ARACHNUT_WALK = 0;
const int A_ARACHNUT_STUNNED = 4;

const int WALK_SPEED = 30;

CArachnut::CArachnut(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y)
{
	setupGalaxyObjectOnMap( 0x2378, A_ARACHNUT_WALK );
	m_hDir = LEFT;

	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	performCollisions();
	processMove( 0, rSprite.m_bboxY1-rSprite.m_bboxY2 );
	processActionRoutine();
}

bool CArachnut::isNearby(CSpriteObject &theObject)
{
	if( !getProbability(80) )
		return false;

	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		if( player->getXMidPos() < getXMidPos() )
			m_hDir = LEFT;
		else
			m_hDir = RIGHT;

		if( player->getYMidPos() < getYMidPos() )
			m_vDir = UP;
		else
			m_vDir = DOWN;
	}

	return true;
}

void CArachnut::getTouchedBy(CSpriteObject &theObject)
{
	if( theObject.dead )
		return;

	if( CBullet *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		setAction(A_ARACHNUT_STUNNED);
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}

void CArachnut::process()
{
	performCollisions();
	processFalling();

	if( blockedl )
		m_hDir = RIGHT;
	else if( blockedr )
		m_hDir = LEFT;

	if( getActionStatus(A_ARACHNUT_WALK) )
		setAction(A_ARACHNUT_WALK);

	if( getActionNumber(A_ARACHNUT_WALK) )
	{
		// Move normally in the direction
		if( m_hDir == RIGHT )
			moveRight( WALK_SPEED );
		else
			moveLeft( WALK_SPEED );
	}


	if(!processActionRoutine())
			exists = false;
}

} /* namespace galaxy */
