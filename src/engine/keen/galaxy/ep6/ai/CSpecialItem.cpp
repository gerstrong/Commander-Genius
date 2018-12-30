#include "CSpecialItem.h"
#include "../../common/ai/CPlayerBase.h"
#include "../../common/dialog/CMessageBoxBitmapGalaxy.h"

namespace galaxy 
{
    
CSpecialItem::CSpecialItem(CMap* pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int actionoffset) : 
CGalaxySpriteObject(pmap, foeID, x, y, 0),
mTaken(false)
{
	setupGalaxyObjectOnMap(actionoffset, 0);
		
	answermap[0] = "KEEN_GOT_GRAPPLING_HOOK";	
	answermap[1] = "KEEN_GOT_SANDWICH";
	answermap[2] = "KEEN_GOT_SHIPCARD";	
}

void CSpecialItem::getTouchedBy(CSpriteObject& theObject)
{
    if(mTaken)
    {
        return;
    }
    
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
	    mTaken = true;
		auto &item = player->m_Inventory.Item.m_special;

		// Sound play
		gSound.playSound(SOUND_GET_SPECIAL_ITEM, SoundPlayMode::PLAY_PAUSEALL);

		switch(mFoeID)
		{
		    case 0x63: item.hook++; break;
		    case 0x64: item.sandwich++; break;
		    case 0x65: item.rocketKeycard++; break;
		    default: break;
		}

        auto exitEvent = new EventExitLevel(mpMap->getLevel(), true, false, player->getSpecialIdx());

        // Play it but afterwards
        exitEvent->playSound = true;                

        // Show got item message
        showMsgWithBmp( player->getSpriteVariantIdx(),
                        gBehaviorEngine.getString(answermap[mFoeID-0x63]),
                        30, LEFT, false, exitEvent );

        player->m_Inventory.Item.m_gem.clear();
	}    
}

void CSpecialItem::process()
{
    if(!processActionRoutine())
	exists = false;    
}

    
};
