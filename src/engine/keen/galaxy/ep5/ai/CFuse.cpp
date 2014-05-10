/*
 * CAmpton.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CFuse.h"
#include "../../common/ai/CPlayerLevel.h"
#include <base/utils/misc.h>

/*
$3186W #QED?
 */


namespace galaxy {  
  
  
CFuse::CFuse(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
mTimer(0)
{  
	// Adapt this AI
	m_ActionBaseOffset = 0x3186;
	setActionForce(0);
	//setupGalaxyObjectOnMap(0x3186, 0);
	
	xDirection = LEFT;
	
	mp_Map->mNumFuses++;
	mp_Map->mFuseInLevel = true;
}


void CFuse::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	/*if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		player->kill();
	}*/
}


void CFuse::process()
{
	//if(!processActionRoutine())
	  //  exists = false;
}

}
