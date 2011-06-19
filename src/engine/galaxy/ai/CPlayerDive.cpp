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
		Cheatmode)
{
	m_ActionBaseOffset = 0x19EC;
	setActionForce(A_KEENSWIM_MOVE);

	processActionRoutine();
	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	processMove(0, (14<<STC)-(rSprite.m_bboxY2-rSprite.m_bboxY1));
	performCollisions();
}

const int movespeed = 40;
const int WATERFALLSPEED = 20;

void CPlayerDive::process()
{
	// In case noclipping was triggered, make it solid, or remove it...
	if(m_Cheatmode.noclipping)
	{
		solid = !solid;
		m_Cheatmode.noclipping = false;
	}

	// Swimming
	if(g_pInput->getHoldedCommand(IC_LEFT))
	{
		if(!g_pInput->getHoldedCommand(IC_UP) && !g_pInput->getHoldedCommand(IC_DOWN))
			m_vDir = NONE;

		moveLeft(movespeed);
		m_hDir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT))
	{
		if(!g_pInput->getHoldedCommand(IC_UP) && !g_pInput->getHoldedCommand(IC_DOWN))
			m_vDir = NONE;

		moveRight(movespeed);
		m_hDir = RIGHT;
	}


	// perform actions depending on if the jump button was pressed
	if(g_pInput->getHoldedCommand(IC_JUMP))
	{
		if(g_pInput->getHoldedCommand(IC_UP))
		{
			//if(!g_pInput->getHoldedCommand(IC_LEFT) && !g_pInput->getHoldedCommand(IC_RIGHT))

			moveUp(movespeed);
			m_vDir = UP;
		}
	}

	moveDown(WATERFALLSPEED);

	m_camera.process();
	m_camera.processEvents();

	processActionRoutine();
}

}
