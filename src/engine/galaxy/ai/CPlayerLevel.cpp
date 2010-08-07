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

const Uint16 MAX_JUMPHEIGHT = (10<<CSF);

CPlayerLevel::CPlayerLevel(CMap *pmap, Uint32 x, Uint32 y,
						std::vector<CObject*>& ObjectPtrs, direction_t facedir) :
CObject(pmap, x, y, OBJ_NONE),
m_basesprite( (facedir==LEFT) ? STANDBASEFRAMELEFT : STANDBASEFRAMERIGHT ),
m_looking_dir(facedir),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_ObjectPtrs(ObjectPtrs)
{
	state = STANDING;
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

	processMoving();

	processJumping();

	processFalling();

	processExiting();
}

void CPlayerLevel::processFalling()
{
	CObject::processFalling();

	// If yinertia is high, set falling to true
	if( yinertia > 64 )
	{
		state = FALLING;
		falling = true;
	}

	if( !blockedd && (falling || state == JUMPING) )
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

	if( blockedd )
	{
		state = STANDING;
	}
}

void CPlayerLevel::processMoving()
{
	size_t movespeed = 50;
	bool walking=false;

	// Normal walking
	if(g_pInput->getHoldedCommand(IC_LEFT) && !blockedl)
	{
		moveLeft(movespeed);
		if(state == STANDING)
			state=WALKING;
		m_looking_dir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT) && !blockedr)
	{
		moveRight(movespeed);
		if(state == STANDING)
			state=WALKING;
		m_looking_dir = RIGHT;
	}

	if( state==WALKING or state==STANDING )
		performWalkingAnimation(walking);
}

void CPlayerLevel::processJumping()
{
	if(g_pInput->getPressedCommand(IC_JUMP) /*&& (state==STANDING or state==WALKING)*/ )
	{
		yinertia = -128;
		state = JUMPING;
	}
}

void CPlayerLevel::performWalkingAnimation(bool walking)
{
	if(m_looking_dir == LEFT)
		m_basesprite = STANDBASEFRAMELEFT;
	else if(m_looking_dir == RIGHT)
		m_basesprite = STANDBASEFRAMERIGHT;

	sprite = m_basesprite;

	if( state == WALKING )
	{
		m_animation_time = 5;
		sprite +=  (m_animation%4)+1;
	}
}

// Processes the exiting of the player. Here all cases are held
void CPlayerLevel::processExiting()
{
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	Uint32 x = getXMidPos();
	if( ((mp_Map->m_width-2)<<CSF) < x || (2<<CSF) > x )
	{
		EventContainer.add( EXIT_LEVEL, mp_Map->getLevel() );
	}
}

CPlayerLevel::~CPlayerLevel() {
	// TODO Auto-generated destructor stub
}

}
