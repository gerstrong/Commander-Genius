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
#include "sdl/input/CInput.h"
#include "graphics/effects/CScrollEffect.h"
#include "CStatusScreenGalaxyEp4.h"

#include "CStatusScreenGalaxyEp5.h"
//#include "CStatusScreenGalaxyEp6.h"


CInventory::CInventory(const std::string& levelname) :
m_HUD(Item.m_points, Item.m_lifes, Item.m_bullets),
mp_StatusScreen(NULL),
mp_StatusBgrnd(NULL),
m_LevelName(levelname)
{
	reset();

	int Episode = g_pBehaviorEngine->getEpisode();

	if(Episode == 4)
		mp_StatusScreen = new CStatusScreenGalaxyEp4(Item, m_LevelName);
	else if(Episode == 5)
		mp_StatusScreen = new CStatusScreenGalaxyEp5(Item, m_LevelName);
//	else if(Episode == 6)
//		mp_StatusScreen = new CStatusScreenGalaxyEp6(Item, m_LevelName);
}

void CInventory::reset()
{
	Item.reset();
}

void CInventory::toggleStatusScreen()
{
	mp_StatusScreen->m_showstatus = !mp_StatusScreen->m_showstatus;

	if(mp_StatusScreen->m_showstatus)
	{
		int scroll_pos = 0;

		mp_StatusScreen->GenerateStatus();
		g_pVideoDriver->collectSurfaces();
		mp_StatusBgrnd = SDL_DisplayFormat(g_pVideoDriver->mp_VideoEngine->getBlitSurface());

		CScrollEffect* ScrollEffect = dynamic_cast<CScrollEffect*>(g_pGfxEngine->Effect());

		if( ScrollEffect )
			scroll_pos = ScrollEffect->getScrollPosition();

		g_pGfxEngine->setupEffect(new CScrollEffect(mp_StatusScreen->getStatusSfc(), mp_StatusBgrnd, scroll_pos, 16));
	}
	else
	{
		// Check if it's already scrolling and get the position
		int scroll_pos = 160;

		CScrollEffect* ScrollEffect = dynamic_cast<CScrollEffect*>(g_pGfxEngine->Effect());

		if( ScrollEffect )
			scroll_pos = ScrollEffect->getScrollPosition();

		g_pGfxEngine->setupEffect(new CScrollEffect(mp_StatusScreen->getStatusSfc(), mp_StatusBgrnd, scroll_pos, -16));
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

void CInventory::operator<<(CSaveGameController &savedGame)
{
	savedGame.decodeData(Item);
}


