/*
 * CAmpton.cpp
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 */


#include "CFuse.h"
#include "engine/galaxy/common/ai/CPlayerBase.h"
#include <engine/galaxy/common/ai/CPlayerLevel.h>
#include "misc.h"

/*
$21DCW #Lil Ampton walk
$21FAW #Lil Ampton walk
$2218W #Lil Ampton walk
$2236W #Lil Ampton walk
$2254W #Lil Ampton turn 4
$2272W #Lil Ampton start pole slide 5
$2290W #Lil Ampton start pole slide
$22AEW #Lil Ampton pole slide 6
$22CCW #Lil Ampton stop pole slide 7
$22EAW #Lil Ampton stop pole slide
$2308W #Lil Ampton flip switch 8
$2326W #Lil Ampton flip switch 
$2344W #Lil Ampton flip switch
$2362W #Lil Ampton flip switch
$2380W #Lil Ampton flip switch
$239EW #Stunned Ampton 13
 */


namespace galaxy {  
  
  
CFuse::CFuse(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CGalaxySpriteObject(pmap, foeID, x, y),
mTimer(0)
{  
	// Adapt this AI
	setupGalaxyObjectOnMap(0x3186, 0);
	
	xDirection = LEFT;
}


void CFuse::getTouchedBy(CSpriteObject &theObject)
{
	if(dead || theObject.dead)
		return;

	/*if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		player->kill();
	}*/
}


void CFuse::process()
{
	if(!processActionRoutine())
	    exists = false;
}

}
