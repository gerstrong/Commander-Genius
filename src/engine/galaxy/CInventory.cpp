/*
 * CInventory.cpp
 *
 *  Created on: 25.11.2010
 *      Author: gerstrong
 */

#include "CInventory.h"
#include "common/CBehaviorEngine.h"
#include "CStatusScreenGalaxyEp4.h"
//#include "CStatusScreenGalaxyEp5.h"
//#include "CStatusScreenGalaxyEp6.h"


CInventory::CInventory(Uint8& difficulty) :
Item(difficulty),
m_HUD(Item.m_points, Item.m_lifes, Item.m_bullets),
mp_StatusScreen(NULL)
{
	reset();

	int Episode = g_pBehaviorEngine->getEpisode();

	if(Episode == 4)
		mp_StatusScreen = new CStatusScreenGalaxyEp4(Item);
//	else if(Episode == 5)
//		mp_StatusScreen = new CStatusScreenGalaxyEp5(Item);
//	else if(Episode == 6)
//		mp_StatusScreen = new CStatusScreenGalaxyEp6(Item);
	mp_StatusScreen->GenerateStatus();
}

void CInventory::reset()
{
	Item.reset();
}

void CInventory::toggleStatusScreen()
{
	mp_StatusScreen->m_showstatus = !mp_StatusScreen->m_showstatus;

	if(mp_StatusScreen->m_showstatus)
		mp_StatusScreen->GenerateStatus();
}


void CInventory::drawHUD()
{
	m_HUD.render();
}

void CInventory::drawStatus()
{
	mp_StatusScreen->draw();
}


