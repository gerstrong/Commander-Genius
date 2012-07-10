/*
 * CEgg.cpp
 *
 *  Created on: 10.07.2012
 *      Author: gerstong
 */

#include "CEgg.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "engine/galaxy/ai/CBullet.h"

namespace galaxy
{


const int A_EGG_NORMAL = 0;
const int A_EGG_BROKEN = 1;
const int A_EGG_SHELL = 2;


CEgg::CEgg(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y)
{
	setupGalaxyObjectOnMap(0x2120, A_EGG_NORMAL);
}

void CEgg::hatch()
{
	setAction(A_EGG_BROKEN);
	setActionSprite();
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

	processFalling();

	performCollisions();

	//processActionRoutine();

}

} /* namespace galaxy */
