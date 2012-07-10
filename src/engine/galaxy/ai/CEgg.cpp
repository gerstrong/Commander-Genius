/*
 * CEgg.cpp
 *
 *  Created on: 10.07.2012
 *      Author: gerstong
 */

#include "CEgg.h"
#include "engine/galaxy/ai/CPlayerLevel.h"
#include "engine/galaxy/ai/CBullet.h"

namespace galaxy {

CEgg::CEgg(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y)
{}

void CEgg::hatch()
{

}


void CEgg::getTouchedBy(CSpriteObject &theObject)
{
	if(theObject.dead )
		return;

	if( CBullet *bullet = dynamic_cast<CBullet*>(&theObject) )
	{
		bullet->setAction(A_KEENSHOT_IMPACT);
		bullet->playSound( SOUND_SHOT_HIT );
		bullet->dead = true;
		hatch();
	}

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		hatch();
	}
}

} /* namespace galaxy */
