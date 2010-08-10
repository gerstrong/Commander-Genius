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

const Uint16 MAX_JUMPHEIGHT = 30;
const Uint16 MIN_JUMPHEIGHT = 10;

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
	m_index = 0;
	state = STANDING;
	sprite = m_basesprite;

	memset(m_playcontrol, 0,PA_MAX_ACTIONS);

	m_pfiring = false;
	m_jumpheight = 0;

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

	processInput();

	performCollisionsSameBox();

	processMoving();

	processJumping();

	processFalling();

	processExiting();
}

void CPlayerLevel::processInput()
{
	m_playcontrol[PA_X] = 0;
	m_playcontrol[PA_Y] = 0;

	if(g_pInput->getHoldedCommand(m_index, IC_LEFT))
		m_playcontrol[PA_X] -= 100;
	if(g_pInput->getHoldedCommand(m_index, IC_RIGHT))
		m_playcontrol[PA_X] += 100;

	if(g_pInput->getHoldedCommand(m_index, IC_UP))
		m_playcontrol[PA_Y] -= 100;
	if(g_pInput->getHoldedCommand(m_index, IC_DOWN))
		m_playcontrol[PA_Y] += 100;

	if(!m_pfiring)
	{
		if(g_pInput->getHoldedCommand(m_index, IC_JUMP))
		{
			m_playcontrol[PA_JUMP]++;
			if(m_jumpheight >= (MAX_JUMPHEIGHT-2))
			{
				m_playcontrol[PA_JUMP] = 0;
				g_pInput->flushCommand(m_index, IC_JUMP);
			}
		}
		else
			m_playcontrol[PA_JUMP] = 0;

		m_playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(m_index, IC_POGO)   ? 1 : 0;
	}

	m_playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(m_index, IC_FIRE)   ? 1 : 0;
	m_playcontrol[PA_STATUS] = g_pInput->getHoldedCommand(m_index, IC_STATUS) ? 1 : 0;

	// The possibility to charge jumps. This is mainly used for the pogo.
	if( m_playcontrol[PA_JUMP] > 50) m_playcontrol[PA_JUMP] = 50;

	if(g_pInput->getTwoButtonFiring(m_index))
	{
		if(m_playcontrol[PA_FIRE])
		{
			m_playcontrol[PA_FIRE] = 0;
		}
		else if(m_playcontrol[PA_JUMP] && m_playcontrol[PA_POGO])
		{
			m_playcontrol[PA_FIRE] = 1;
			m_playcontrol[PA_JUMP] = 0;
			m_playcontrol[PA_POGO] = 0;
		}
	}
}

void CPlayerLevel::processFalling()
{
	CObject::processFalling();

	// If yinertia is high, set falling to true
	if( yinertia > 0 && !onslope )
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
	if(state != JUMPING)
	{
		if(blockedd)
			m_jumpheight = 0;

		// Not jumping? Let's if we can prepare the player to do so
		if(m_playcontrol[PA_JUMP] && (state==STANDING or state==WALKING) )
		{
			yinertia = -136;
			state = JUMPING;
		}
	}
	else
	{
		// while button is pressed, make the player jump higher
		if(m_playcontrol[PA_JUMP] || m_jumpheight <= MIN_JUMPHEIGHT)
			m_jumpheight++;
		else
			m_jumpheight = 0;

		// If the max. height is reached or the player cancels the jump by release the button
		// make keen fall
		if( m_jumpheight == 0 || m_jumpheight >= MAX_JUMPHEIGHT )
		{
			yinertia = 0;
			m_jumpheight = 0;
		}

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
		EventContainer.add( new EventExitLevel(mp_Map->getLevel()) );
	}
}

CPlayerLevel::~CPlayerLevel() {
	// TODO Auto-generated destructor stub
}

}
