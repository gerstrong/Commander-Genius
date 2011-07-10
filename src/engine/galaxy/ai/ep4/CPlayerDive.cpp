/*
 * CPlayerDive.cpp
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 */

#include "CPlayerDive.h"
#include "sdl/CInput.h"


namespace galaxy {

CPlayerDive::CPlayerDive(CMap *pmap,
		Uint32 x,
		Uint32 y,
		std::vector<CObject*>& ObjectPtrs,
		direction_t facedir,
		CInventory &l_Inventory,
		stCheat &Cheatmode) :
CPlayerBase(pmap, x, y,
		ObjectPtrs,
		facedir,
		l_Inventory,
		Cheatmode),
m_swimupspeed(0),
m_breathtimer(0)
{
	setupGalaxyObjectOnMap(0x19EC, A_KEENSWIM_MOVE);
}




const int MAXMOVESPEED = 15;
const int MOVESPEED = 40;
const int WATERFALLSPEED = 20;
const int BREATH_TIME = 60;

void CPlayerDive::process()
{
	// In case noclipping was triggered, make it solid, or remove it...
	if(m_Cheatmode.noclipping)
	{
		solid = !solid;
		m_Cheatmode.noclipping = false;
	}


	// If Player presses Jump button, make Keen swim faster
	if(g_pInput->getPressedCommand(IC_JUMP))
	{
		// Slow down the swim speed, by time

		if(getActionNumber(A_KEENSWIM_MOVE))
			setActionForce(A_KEENSWIM_MOVE+1);
		else
			setActionForce(A_KEENSWIM_MOVE);

		if(m_swimupspeed<MAXMOVESPEED)
			m_swimupspeed = MAXMOVESPEED;
	}

	// Swimming - Left and Right
	if(g_pInput->getHoldedCommand(IC_LEFT))
	{
		if(!g_pInput->getHoldedCommand(IC_UP) && !g_pInput->getHoldedCommand(IC_DOWN))
			m_vDir = NONE;

		moveLeft(MOVESPEED+m_swimupspeed);
		m_hDir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT))
	{
		if(!g_pInput->getHoldedCommand(IC_UP) && !g_pInput->getHoldedCommand(IC_DOWN))
			m_vDir = NONE;

		moveRight(MOVESPEED+m_swimupspeed);
		m_hDir = RIGHT;
	}

	// Up and down swimming
	if( m_swimupspeed>0 && g_pInput->getHoldedCommand(IC_UP))
	{
		moveUp(MOVESPEED+m_swimupspeed);
		m_vDir = UP;
	}
	else
	{
		moveDown(WATERFALLSPEED+m_swimupspeed);
		m_vDir = DOWN;
	}


	// Slow down the swim speed, by time
	if(m_swimupspeed>0)
	{
		m_swimupspeed--;
		if(m_swimupspeed<0)
			m_swimupspeed = 0;
	}

	if( m_breathtimer >= BREATH_TIME )
	{
		playSound(SOUND_BUBBLE);
		m_breathtimer = 0;
	}
	else
		m_breathtimer++;

	processLevelMiscFlagsCheck();

	m_camera.process();
	m_camera.processEvents();

	processActionRoutine();
}

}
