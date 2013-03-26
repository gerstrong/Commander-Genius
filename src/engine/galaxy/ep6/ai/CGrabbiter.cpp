#include "CGrabbiter.h"
#include <engine/galaxy/common/ai/CPlayerBase.h>

namespace galaxy
{
    

enum GRABBITERACTIONS
{
    A_GRABBITER_HUNGRY = 0,    
    A_GRABBITER_NAPPING = 2,
};


CGrabbiter::CGrabbiter(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) : 
CStunnable(pmap, foeID, x, y)
{
	mActionMap[A_GRABBITER_HUNGRY] = (void (CStunnable::*)()) &CGrabbiter::processHungry;
	mActionMap[A_GRABBITER_NAPPING] = (void (CStunnable::*)()) &CGrabbiter::processNapping;
	
	setupGalaxyObjectOnMap(0x1A90, A_GRABBITER_HUNGRY);
	honorPriority = false;
}


void CGrabbiter::processHungry()
{

}


void CGrabbiter::processNapping()
{

}


void CGrabbiter::getTouchedBy(CSpriteObject& theObject)
{
    if(getActionNumber(A_GRABBITER_NAPPING))
    {
	return;
    }
    
    
    if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
    {
	const int diffX = getXMidPos()-player->getXMidPos();
        player->moveXDir(-diffX/4);

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	
	if(player->m_Inventory.Item.m_special.ep6.sandwich > 0)
	{
	   player->m_Inventory.Item.m_special.ep6.sandwich--; 
	   
	    // Show grabbiter message
	    EventContainer.add( new EventSendDialog("Yummy!") );
	    setAction(A_GRABBITER_NAPPING);
	}
	else
	{	    
	    // Sound play
	    g_pSound->playSound(SOUND_GET_SPECIAL_ITEM, PLAY_PAUSEALL);	
	    
	    // Show grabbiter message
	    EventContainer.add( new EventSendDialog("I'm hungry. You can't pass")  );
	    
	}
    }    
}


void CGrabbiter::process()
{
	performCollisions();
	
	if(!processActionRoutine())
	    exists = false;
	
	(this->*mp_processState)();    
}

}