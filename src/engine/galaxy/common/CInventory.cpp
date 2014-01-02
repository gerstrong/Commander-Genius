/*
 * CInventory.cpp
 *
 *  Created on: 25.11.2010
 *      Author: gerstrong
 */

#include "CInventory.h"
#include "common/CBehaviorEngine.h"
#include "graphics/CGfxEngine.h"
#include "sdl/CVideoDriver.h"
#include <base/CInput.h>
#include "graphics/effects/CScrollEffect.h"


#include "engine/galaxy/ep4/CStatusScreenGalaxyEp4.h"
#include "engine/galaxy/ep5/CStatusScreenGalaxyEp5.h"
#include "engine/galaxy/ep6/CStatusScreenGalaxyEp6.h"


CInventory::CInventory(const int id) :
m_HUD(Item.m_points, Item.m_lifes, Item.m_bullets, id),
mp_StatusBgrnd(NULL)
{
	reset();

	int Episode = g_pBehaviorEngine->getEpisode();

	if(Episode >= 4)
	{
	    const Difficulty difficulty = g_pBehaviorEngine->mDifficulty;
	    
	    Item.m_bullets = 8;
	    if( difficulty > EASY )
		Item.m_bullets = 5;
	
	    if(Episode == 4)
	    {
            mp_StatusScreen.reset(new CStatusScreenGalaxyEp4(Item));
	    }
	    else if(Episode == 5)
	    {
            mp_StatusScreen.reset(new CStatusScreenGalaxyEp5(Item));
	    }
	    else if(Episode == 6)
	    {
            mp_StatusScreen.reset(new CStatusScreenGalaxyEp6(Item));
	    }
	}
	
	m_HUD.sync();
}


void CInventory::reset()
{
	Item.reset();
}


void CInventory::fetchImportantStuff(const CInventory &other)
{
    Item.m_special.ep4.swimsuit |= other.Item.m_special.ep4.swimsuit;
    Item.m_special.ep4.elders += other.Item.m_special.ep4.elders;
    Item.m_special.ep6.sandwich |= other.Item.m_special.ep6.sandwich;
    Item.m_special.ep6.hook |= other.Item.m_special.ep6.hook;
    Item.m_special.ep6.rocketKeycard |= other.Item.m_special.ep6.rocketKeycard;
}


void CInventory::toggleStatusScreen()
{
	mp_StatusScreen->m_showstatus = !mp_StatusScreen->m_showstatus;

	if(mp_StatusScreen->m_showstatus)
	{
		int scroll_pos = 0;

		mp_StatusScreen->GenerateStatus();
		g_pVideoDriver->collectSurfaces();

        mp_StatusBgrnd = g_pVideoDriver->convertThroughBlitSfc(g_pVideoDriver->mpVideoEngine->getBlitSurface());

		CScrollEffect* ScrollEffect = dynamic_cast<CScrollEffect*>(g_pGfxEngine->Effect());

		if( ScrollEffect )
			scroll_pos = ScrollEffect->getScrollPosition();

        g_pGfxEngine->setupEffect(new CScrollEffect(mp_StatusScreen->getStatusSfc(), scroll_pos, 10, CENTER, DOWN));
	}
	else
	{		
		int scroll_pos = 160;

		CScrollEffect* ScrollEffect = dynamic_cast<CScrollEffect*>(g_pGfxEngine->Effect());

        // Check if it's already scrolling and get the position
		if( ScrollEffect )
			scroll_pos = ScrollEffect->getScrollPosition();

        g_pGfxEngine->setupEffect(new CScrollEffect(mp_StatusScreen->getStatusSfc(), scroll_pos, -10, CENTER, DOWN));
		if(mp_StatusBgrnd)
			SDL_FreeSurface(mp_StatusBgrnd);
		mp_StatusBgrnd = NULL;
	}

	g_pInput->flushAll();
}


void CInventory::drawHUD()
{
    m_HUD.render();
}

void CInventory::setup(const int sprVar)
{
    m_HUD.setup(sprVar);
}

void CInventory::drawStatus()
{
	if(g_pGfxEngine->applyingEffects())
		return;

	mp_StatusScreen->draw();
}

void CInventory::operator>>(CSaveGameController &savedGame)
{
	savedGame.encodeData(Item);
}

void CInventory::operator>>(boost::property_tree::ptree &invNode)
{
    Item >> invNode;
}

void CInventory::operator<<(CSaveGameController &savedGame)
{
	savedGame.decodeData(Item);
}

void CInventory::operator<<(boost::property_tree::ptree &invNode)
{
    Item << invNode;
}


