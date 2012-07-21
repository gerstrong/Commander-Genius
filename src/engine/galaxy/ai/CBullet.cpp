/*
 * CBullet.cpp
 *
 *  Created on: 01.12.2010
 *      Author: gerstrong
 */

#include "CBullet.h"
#include "sdl/sound/CSound.h"

namespace galaxy
{

const Uint32 speed = 10;
const Uint32 hittime = 100;

CBullet::CBullet(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int xDir, const int yDir) :
CGalaxySpriteObject(pmap, foeID, x, y)
{
	xDirection = xDir;
	yDirection = yDir;

	setupGalaxyObjectOnMap(0x1738, A_KEENSHOT_MOVING);
	setActionSprite();
	calcBoundingBoxes();
	playSound( SOUND_KEEN_FIRE );
}

void CBullet::process()
{
	if( !getActionNumber(A_KEENSHOT_IMPACT) && (blockedd || blockedu || blockedl || blockedr || onslope) )
	{
		setAction(A_KEENSHOT_IMPACT);
		playSound( SOUND_SHOT_HIT );
		dead = true;
	}

	if(!processActionRoutine())
			exists = false;

}

}
