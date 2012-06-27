/*
 * CSlugSlime.cpp
 *
 *  Created on: 28.05.2011
 *      Author: gerstrong
 */

#include "CSlugSlime.h"
#include "engine/galaxy/ai/CPlayerBase.h"

namespace galaxy
{


const int A_SLUGPOISON_NORMAL =	0;
const int A_SLUGPOISON_FADE = 1;

CSlugSlime::CSlugSlime(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y)
{
	setupGalaxyObjectOnMap(0x20A8, A_SLUGPOISON_NORMAL);
}

void CSlugSlime::getTouchedBy(CSpriteObject &theObject)
{
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}
}


void CSlugSlime::process()
{
	processFalling();
	processActionRoutine();
}

};
