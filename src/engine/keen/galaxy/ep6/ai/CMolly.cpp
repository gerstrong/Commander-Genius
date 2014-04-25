#include "CMolly.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"
#include "engine/core/mode/CGameMode.h"

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
	performCliffStop(m_Action.velX<<1);

	// Move normally in the direction
	if( xDirection == RIGHT )
		moveRight( m_Action.velX<<1 );
	else
		moveLeft( m_Action.velX<<1 );
}

void CMolly::getTouchedBy(CSpriteObject& theObject)
{
	if(rescued)
		return;

	// When Keen touches the Council Member exit the level and add one to the council list
	//if( typeid(theObject) == typeid(CPlayerLevel) )
	if( /*CPlayerBase *player = */dynamic_cast<CPlayerBase*>(&theObject) )
	{
        //CEventContainer& EventContainer = gEventManager;

        std::vector<CMessageBoxGalaxy*> msgs;

        msgs.push_back( new CMessageBoxBitmapGalaxy( "Thanks for the rescue", gGraphics.getBitmapFromId(3), LEFT) );
        msgs.push_back( new CMessageBoxBitmapGalaxy( "Null Problemo", *gGraphics.getBitmapFromStr("KEENTHUMBSUP"), RIGHT ) );

        const std::string end_text("End of Episode.\n"
                       "The game will be restarted.\n"
                       "You can replay it again or\n"
                       "try another Episode for more fun!\n"
                       "The original epilog is under construction.");

        msgs.push_back( new CMessageBoxGalaxy( end_text, new EventEndGamePlay() ) );

        showMsgVec(msgs);

		rescued = true;
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
