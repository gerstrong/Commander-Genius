#include "CSpecialItem.h"
#include <engine/galaxy/common/ai/CPlayerBase.h>

namespace galaxy 
{
    
CSpecialItem::CSpecialItem(CMap* pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int actionoffset) : 
CGalaxySpriteObject(pmap, foeID, x, y)
{
	setupGalaxyObjectOnMap(actionoffset, 0);
	
	answermap[1] = "KEEN_GOT_GRAPPLING_HOOK";
	answermap[0] = "KEEN_GOT_SANDWICH";	
	answermap[2] = "KEEN_GOT_SHIPCARD";	
}

void CSpecialItem::getTouchedBy(CSpriteObject& theObject)
{
    
	if( CPlayerBase *player = dynamic_cast<CPlayerBase*>(&theObject) )
	{
		auto &item = player->m_Inventory.Item.m_special.ep6;

		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

		// TODO: Sound play
		//g_pSound->playSound(SOUND_GOT_SPECIAL_ITEM, PLAY_PAUSEALL);


		// TODO: Show message(s)
		/*std::vector< std::shared_ptr<EventSendBitmapDialogMsg> > msgs;

		std::unique_ptr<EventSendBitmapDialogMsg> msg1(new EventSendBitmapDialogMsg(g_pGfxEngine->getBitmap(104), elder_text[0], LEFT));
		std::unique_ptr<EventSendBitmapDialogMsg> msg2(new EventSendBitmapDialogMsg(*g_pGfxEngine->getBitmap("KEENTHUMBSUP"), elder_text[1], RIGHT));
		msgs.push_back( move(msg1) );
		msgs.push_back( move(msg2) );

		EventContainer.add( new EventSendBitmapDialogMessages(msgs) );*/
		
		switch(mFoeID)
		{
		    case 0x63: item.hook++; break;
		    case 0x64: item.sandwich++; break;
		    case 0x65: item.rocketKeycard++; break;
		    default: break;
		}
		
		EventContainer.add( new EventExitLevel(mp_Map->getLevel(), true) );		    
		
		player->m_Inventory.Item.m_gem.empty();
	}    
}

void CSpecialItem::process()
{
    if(!processActionRoutine())
	exists = false;    
}

    
};