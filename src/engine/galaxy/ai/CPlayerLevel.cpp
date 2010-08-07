/*
 * CPlayerLevel.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CPlayerLevel.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CInput.h"
#include "CVec.h"


namespace galaxy {

const Uint16 STANDBASEFRAMELEFT = 14;
const Uint16 STANDBASEFRAMERIGHT = 6;

const Uint16 FALLINGFRAMELEFT = 19;
const Uint16 FALLINGFRAMERIGHT = 11;

CPlayerLevel::CPlayerLevel(CMap *pmap, Uint32 x, Uint32 y,
						std::vector<CObject*>& ObjectPtrs) :
CObject(pmap, x, y, OBJ_PLAYER),
m_basesprite(STANDBASEFRAMELEFT),
m_looking_dir(LEFT),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_ObjectPtrs(ObjectPtrs)
{
	jumping = false;
	// TODO Auto-generated constructor stub
	sprite = m_basesprite;

	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	moveUp(rSprite.m_bboxY2-rSprite.m_bboxY1+(1<<CSF));

	performCollisions();
}

void CPlayerLevel::process()
{
	// Perform animation cycle
	if(m_animation_ticker >= m_animation_time)
	{
		m_animation++;
		m_animation_ticker = 0;
	}
	else m_animation_ticker++;

	performCollisionsSameBox();

	processWalking();

	processFalling();

	processJumping();
}

void CPlayerLevel::processFalling()
{
	CObject::processFalling();

	// If yinertia is high, set falling to true
	if( yinertia > 64 )
	{
		falling = true;
		jumping = false;
	}

	if( !blockedd && (falling || jumping) )
	{
		if(m_looking_dir == LEFT)
			m_basesprite = FALLINGFRAMELEFT;
		else if(m_looking_dir == RIGHT)
			m_basesprite = FALLINGFRAMERIGHT;

		sprite = m_basesprite;

		if( yinertia > -32 ) sprite++;
		if( yinertia > 32 )
			sprite++;
	}
}

void CPlayerLevel::processWalking()
{
	size_t movespeed = 50;
	bool walking=false;

	// Normal walking
	if(g_pInput->getHoldedCommand(IC_LEFT) && !blockedl)
	{
		moveLeft(movespeed);
		walking = true;
		m_looking_dir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT) && !blockedr)
	{
		moveRight(movespeed);
		walking = true;
		m_looking_dir = RIGHT;
	}

	performWalkingAnimation(walking);
}

void CPlayerLevel::processJumping()
{
	if(g_pInput->getPressedCommand(IC_JUMP) && !falling && !jumping)
	{
		yinertia = -150;
		jumping = true;
	}

}

void CPlayerLevel::performWalkingAnimation(bool walking)
{
	if(m_looking_dir == LEFT)
		m_basesprite = STANDBASEFRAMELEFT;
	else if(m_looking_dir == RIGHT)
		m_basesprite = STANDBASEFRAMERIGHT;

	sprite = m_basesprite;

	if(walking)
	{
		m_animation_time = 5;
		sprite +=  (m_animation%4)+1;
	}
}


CPlayerLevel::~CPlayerLevel() {
	// TODO Auto-generated destructor stub
}

}
