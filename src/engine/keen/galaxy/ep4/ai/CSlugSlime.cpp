/*
 * CSlugSlime.cpp
 *
 *  Created on: 28.05.2011
 *      Author: gerstrong
 */

#include "CSlugSlime.h"
#include "../../common/ai/CPlayerBase.h"

namespace galaxy
{


const int A_SLUGPOISON_NORMAL =	0;
//const int A_SLUGPOISON_FADE = 1;
const int POISON_TIME = 1;

CSlugSlime::CSlugSlime(CMap *pmap,
                       const Uint16 foeID,
                       const int x,
                       const int y,
                       const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar)
{
	setupGalaxyObjectOnMap(0x20A8, A_SLUGPOISON_NORMAL);
}

void CSlugSlime::getTouchedBy(CSpriteObject &theObject)
{
    if(!getActionStatus(A_SLUGPOISON_NORMAL))
    {
        return;
    }

	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
        player->m_Inventory.addAchievementTask("What a poop!", 1);
	}
}


void CSlugSlime::process()
{
	performCollisions();

    performGravityMid();
	
    if( m_timer < POISON_TIME )
    {
        m_timer++;
        return;
    }
    else
    {
        m_timer = 0;
    }	
	
	if(!processActionRoutine())
			exists = false;
}



};




