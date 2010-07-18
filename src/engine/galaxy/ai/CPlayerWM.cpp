/*
 * CPlayer.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 */

#include "CPlayerWM.h"
#include "../../../common/CBehaviorEngine.h"
#include "../../../sdl/CInput.h"

namespace galaxy {

CPlayerWM::CPlayerWM(CMap *pmap, Uint32 x, Uint32 y):
CObject(pmap, x, y, OBJ_PLAYER)
{
	// TODO Auto-generated constructor stub
	sprite=138;

	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	bboxX1 = rSprite.m_bboxX1;		bboxX2 = rSprite.m_bboxX2;
	bboxY1 = rSprite.m_bboxY1;		bboxY2 = rSprite.m_bboxY2;

	checkinitialCollisions();
}

/**
 * The main process cycle for the player itself only on the map
 */
void CPlayerWM::process()
{
	processWalking();
}

/*
 * Processes the walking of the player on map here
 */
void CPlayerWM::processWalking()
{
	size_t movespeed = 100;

	// TODO: Implement animation speed!

	if(g_pInput->getHoldedCommand(IC_LEFT))
		moveLeft(movespeed);
	if(g_pInput->getHoldedCommand(IC_RIGHT))
		moveRight(movespeed);
	if(g_pInput->getHoldedCommand(IC_UP))
		moveUp(movespeed);
	if(g_pInput->getHoldedCommand(IC_DOWN))
		moveDown(movespeed);

	if(g_pInput->getHoldedCommand(IC_STATUS))
		solid = !solid;

}

CPlayerWM::~CPlayerWM() {
	// TODO Auto-generated destructor stub
}

}
