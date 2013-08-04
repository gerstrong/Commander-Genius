/*
 * CDiveSuit.cpp
 *
 *  Created on: 25.06.2011
 *      Author: gerstrong
 */

#include "CDiveSuit.h"
#include "engine/galaxy/common/ai/CPlayerLevel.h"

namespace galaxy {

CDiveSuit::CDiveSuit(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar) :
CGalaxySpriteObject(pmap, foeID, x, y, sprVar),
taken(false)
{    
    sprite = 306;
    calcBoundingBoxes();
    alignToTile();
}

void CDiveSuit::process()
{}

void CDiveSuit::getTouchedBy(CSpriteObject &theObject)
{
	if(taken)
		return;

	// When Keen touches the Council Member exit the level and add one to the council list
	if( CPlayerLevel *player = dynamic_cast<CPlayerLevel*>(&theObject) )
	{
		bool &swimsuit = player->m_Inventory.Item.m_special.ep4.swimsuit;

		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

		g_pSound->playSound( SOUND_GET_WETSUIT, PLAY_PAUSEALL );
		taken = swimsuit = true;

		EventContainer.add( new EventSendBitmapDialogMsg(*g_pGfxEngine->getBitmap("KEENTHUMBSUP"),
				g_pBehaviorEngine->getString("SWIM_SUIT_TEXT"), LEFT) );

		EventContainer.add( new EventExitLevel(mp_Map->getLevel(), true) );
		player->m_Inventory.Item.m_gem.empty();
	}
}


} /* namespace galaxy */
