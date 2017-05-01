#include "CMolly.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"
#include "engine/core/mode/CGameMode.h"
#include "../../menu/ComputerWrist.h"

namespace galaxy 
{
      
/*
$1D60W  #Molly
$1D7EW  #Molly
$1D9CW  #Molly
$1DBAW  #Molly
*/

const int MOLLY_MOVE_SPEED = 1;

CMolly::CMolly(CMap* pmap, const Uint16 foeID, Uint32 x, Uint32 y) : 
CGalaxySpriteObject(pmap, foeID, x, y, 0),
rescued(false)
{
    setupGalaxyObjectOnMap(0x1D60, 0);
}



void CMolly::processMoving()
{
	performCollisions();
	performGravityLow();

	// Check if there is a cliff and move him back in case
	performCliffStop(m_Action.h_anim_move<<1);

	// Move normally in the direction
	if( xDirection == RIGHT )
		moveRight( m_Action.h_anim_move<<1 );
	else
		moveLeft( m_Action.h_anim_move<<1 );
}

void CMolly::getTouchedBy(CSpriteObject& theObject)
{
	if(rescued)
		return;

	// When Keen touches the Council Member exit the level and add one to the council list
    if( dynamic_cast<CPlayerBase*>(&theObject) )
	{
		rescued = true;

        gEventManager.add(new OpenComputerWrist(0));
        gEventManager.add(new EventEndGamePlay());
	}    
}


void CMolly::process()
{
    processMoving();
    
    if( blockedl )
	xDirection = RIGHT;
    else if(blockedr)
	xDirection = LEFT;
    
    if(!processActionRoutine())
	exists = false;    
}


    
};
