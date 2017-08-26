#include "CGrabbiter.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/dialog/CMessageBoxGalaxy.h"

namespace galaxy
{
    

enum GRABBITERACTIONS
{
    A_GRABBITER_HUNGRY = 0,    
    A_GRABBITER_NAPPING = 2
};


CGrabbiter::CGrabbiter(CMap* pmap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pmap, foeID, x, y)
{
	mActionMap[A_GRABBITER_HUNGRY] = (GASOFctr) &CGrabbiter::processHungry;
	mActionMap[A_GRABBITER_NAPPING] = (GASOFctr) &CGrabbiter::processNapping;
	
	setupGalaxyObjectOnMap(0x1A90, A_GRABBITER_HUNGRY);
	honorPriority = false;
}


void
CGrabbiter::
deserialize(CSaveGameController &savedGame)
{
    savedGame.decodeData(m_Pos.x);
    savedGame.decodeData(m_Pos.y);
}

void
CGrabbiter::
serialize(CSaveGameController &savedGame)
{
    savedGame.encodeData(m_Pos.x);
    savedGame.encodeData(m_Pos.y);
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

        if(player->m_Inventory.Item.m_special.ep6.sandwich > 0)
        {
            player->m_Inventory.Item.m_special.ep6.sandwich--;

            // Show grabbiter message
            showMsg( gBehaviorEngine.getString("KEEN_GRABBITER_SLEEPY") );

            setAction(A_GRABBITER_NAPPING);
            playSound(SOUND_GRABBITER_SLEEP);
        }
        else
        {
            // Sound play
            gSound.playSound(SOUND_GRABBITER_HUNGRY, PLAY_PAUSEALL);

            // Show grabbiter message
            showMsg( gBehaviorEngine.getString("KEEN_GRABBITER_HUNGRY") );
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
