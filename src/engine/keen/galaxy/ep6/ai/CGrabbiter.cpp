#include "CGrabbiter.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/dialog/CMessageBoxGalaxy.h"

#include <engine/core/CBehaviorEngine.h>

namespace galaxy
{
    

enum GRABBITERACTIONS
{
    A_GRABBITER_HUNGRY = 0,    
    A_GRABBITER_NAPPING = 2
};


CGrabbiter::CGrabbiter(std::shared_ptr<CMap> pMap, const Uint16 foeID, const Uint32 x, const Uint32 y) :
CStunnable(pMap, foeID, x, y)
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


void
CGrabbiter::serialize(GsKit::ptree &node)
{
    auto &posNode = node.put("pos", "");
    posNode.put("<xmlattr>.x", m_Pos.x);
    posNode.put("<xmlattr>.y", m_Pos.y);
}
void
CGrabbiter::deserialize(GsKit::ptree &node)
{
    auto &posNode = node.put("pos", "");
    m_Pos.x = posNode.get<Uint32>("<xmlattr>.x", m_Pos.x);
    m_Pos.y = posNode.get<Uint32>("<xmlattr>.y", m_Pos.y);
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

        const int sprVar = player->getSpriteVariantIdx();

        if(player->m_Inventory.Item.m_special.sandwich > 0)
        {
            player->m_Inventory.Item.m_special.sandwich--;

            // Show grabbiter message
            showMsg(sprVar, gBehaviorEngine.getString("KEEN_GRABBITER_SLEEPY") );

            setAction(A_GRABBITER_NAPPING);
            playSound(GameSound::GRABBITER_SLEEP);
        }
        else
        {
            // Sound play
            playSound(GameSound::GRABBITER_HUNGRY, SoundPlayMode::PLAY_PAUSEALL);

            // Show grabbiter message
            showMsg(sprVar, gBehaviorEngine.getString("KEEN_GRABBITER_HUNGRY") );
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
