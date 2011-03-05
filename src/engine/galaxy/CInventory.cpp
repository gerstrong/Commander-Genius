/*
 * CInventory.cpp
 *
 *  Created on: 25.11.2010
 *      Author: gerstrong
 */

#include "CInventory.h"
#include "CStatusScreenGalaxy.h"


CInventory::CInventory() :
m_HUD(m_points, m_lifes, m_bullets)
{
	reset();
}

void CInventory::reset()
{
	m_lifes = 3;
	m_points = 0;
	m_lifeAt = 20000;
	m_drops = 0;
	m_bullets = 0;
	m_gem.red = 0;
	m_gem.blue = 0;
	m_gem.green = 0;
	m_gem.yellow = 0;

	m_keycards = 0;

	m_special.ep4.elders = 0;
	m_special.ep4.swimsuit = 0;
}

void CInventory::drawHUD()
{
	m_HUD.render();
}

void CInventory::drawStatus()
{
	m_StatusScreen.draw();
}


