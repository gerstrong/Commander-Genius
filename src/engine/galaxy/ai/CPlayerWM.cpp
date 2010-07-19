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
CObject(pmap, x, y, OBJ_PLAYER),
m_basesprite(130),
m_looking_dir(LEFT),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0)
{
	// TODO Auto-generated constructor stub
	sprite = m_basesprite;

	setupinitialCollisions();
}

/**
 * The main process cycle for the player itself only on the map
 */
void CPlayerWM::process()
{
	// Perform animation cycle
	if(m_animation_ticker >= m_animation_time)
	{
		m_animation++;
		m_animation_ticker = 0;
	}
	else m_animation_ticker++;

	processWalking();
}

/*
 * Processes the walking of the player on map here
 */
void CPlayerWM::processWalking()
{
	size_t movespeed = 100;
	bool walking=false;

	// Normal walking
	if(g_pInput->getHoldedCommand(IC_LEFT))
	{
		moveLeft(movespeed);
		walking = true;
		m_looking_dir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT))
	{
		moveRight(movespeed);
		walking = true;
		m_looking_dir = RIGHT;
	}

	if(g_pInput->getHoldedCommand(IC_UP))
	{
		moveUp(movespeed);
		walking = true;

		if(m_looking_dir == LEFT)
			m_looking_dir = LEFTUP;
		else if(m_looking_dir == RIGHT)
			m_looking_dir = RIGHTUP;
		else
			m_looking_dir = UP;
	}
	else if(g_pInput->getHoldedCommand(IC_DOWN))
	{
		moveDown(movespeed);
		walking = true;

		if(m_looking_dir == LEFT)
			m_looking_dir = LEFTDOWN;
		else if(m_looking_dir == RIGHT)
			m_looking_dir = RIGHTDOWN;
		else
			m_looking_dir = DOWN;
	}

	if(g_pInput->getHoldedCommand(IC_STATUS))
		solid = !solid;

	// perform actions depending if the action button was pressed
	if(g_pInput->getHoldedCommand(IC_JUMP))
	{
		Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
		if(object)
			finishlevel(object);

		g_pInput->flushCommands();
	}

	performWalkingAnimation(walking);






}

/*
 *	makes the player finish the level
 */
void CPlayerWM::finishlevel(Uint16 object)
{
	// if a door or other blocker was found remove it
	int x, y;
	Uint16 door = (object - 0xC000) + 0xD000;
	while(mp_Map->findTile(door, &x, &y, 2))
	{
		mp_Map->setTile( x, y, 0, true, 1);
		mp_Map->setTile( x, y, 0, true, 2);
		mp_Map->redrawAt( x, y);
	}

	// TODO: spawn the flag that will be spawn
}

/**
 * This performs the animation when player is walking on the map
 */
void CPlayerWM::performWalkingAnimation(bool walking)
{
	if(m_looking_dir == LEFT)
		sprite = m_basesprite;
	else if(m_looking_dir == RIGHT)
		sprite = m_basesprite + 3;
	else if(m_looking_dir == UP)
		sprite = m_basesprite + 6;
	else if(m_looking_dir == DOWN)
		sprite = m_basesprite + 9;
	else if(m_looking_dir == RIGHTDOWN)
		sprite = m_basesprite + 12;
	else if(m_looking_dir == LEFTDOWN)
		sprite = m_basesprite + 15;
	else if(m_looking_dir == LEFTUP)
		sprite = m_basesprite + 18;
	else if(m_looking_dir == RIGHTUP)
		sprite = m_basesprite + 21;

	if(walking)
	{
		m_animation_time = 5;
		sprite +=  m_animation%2;
	}
	else
		sprite +=  2;
}

CPlayerWM::~CPlayerWM() {
	// TODO Auto-generated destructor stub
}

}
