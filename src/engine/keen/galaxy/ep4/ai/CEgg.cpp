/*
 * CEgg.cpp
 *
 *  Created on: 10.07.2012
 *      Author: gerstong
 */

#include "CEgg.h"
#include "../../common/ai/CPlayerLevel.h"
#include "../../common/ai/CBullet.h"
#include "CBlueBird.h"

namespace galaxy
{


const int A_EGG_NORMAL = 0;
const int A_EGG_BROKEN = 1;
const int A_EGG_SHELL1 = 2;
const int A_EGG_SHELL2 = 3;
const int A_EGG_SHELL3 = 4;


CEgg::CEgg(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y, 0)
{
	setupGalaxyObjectOnMap(0x2120, A_EGG_NORMAL);
}

void CEgg::hatch()
{
	setAction(A_EGG_BROKEN);
	setActionSprite();

	// Create the shards.
	CEgg *shard1 = new CEgg( getMapPtr(), mFoeID, getXMidPos(), getYMidPos()-(8<<STC) );
	CEgg *shard2 = new CEgg( getMapPtr(), mFoeID, getXMidPos(), getYMidPos()-(8<<STC) );
	CEgg *shard3 = new CEgg( getMapPtr(), mFoeID, getXMidPos(), getYMidPos()-(8<<STC) );

	// Some impulse values for the shards when Egg is shot
	shard1->xinertia =  -70;
	shard1->yinertia =  -60;
	shard2->xinertia = -100;
	shard2->yinertia =  -80;
	shard3->xinertia =  +60;
	shard3->yinertia =  -70;

	shard1->setAction(A_EGG_SHELL1);
	shard2->setAction(A_EGG_SHELL2);
	shard3->setAction(A_EGG_SHELL3);

	shard1->setActionSprite();
	shard2->setActionSprite();
	shard3->setActionSprite();

	gEventManager.add( new EventSpawnObject( shard1 ) );
	gEventManager.add( new EventSpawnObject( shard2 ) );
	gEventManager.add( new EventSpawnObject( shard3 ) );

	CBlueBird *blueBird = new CBlueBird( getMapPtr(), mFoeID, getXMidPos(), getYMidPos() );
	gEventManager.add( new EventSpawnObject( blueBird ) );
}



void CEgg::getTouchedBy(CSpriteObject &theObject)
{
	if( !getActionNumber(A_EGG_NORMAL) || theObject.dead )
		return;

	if( CBullet *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		bullet->dead = true;
		hatch();
	}

	if( dynamic_cast<CPlayerBase*>(&theObject) )
	{
		hatch();
	}
}

void CEgg::process()
{

	performGravityMid();

	performCollisions();

	InertiaAndFriction_X(3);
}

} /* namespace galaxy */
