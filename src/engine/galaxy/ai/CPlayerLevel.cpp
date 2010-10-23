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

// TODO: Needs recoding. This was just for testing, but now we need something much more
// serious. I want to create a diagram to represent all actions that keen can do and code them
// here according to that diagram. Something like UML or Petri. This Code got a bit out of control

namespace galaxy {

const Uint16 MAX_JUMPHEIGHT = 30;
const Uint16 MIN_JUMPHEIGHT = 10;

CPlayerLevel::CPlayerLevel(CMap *pmap, Uint32 x, Uint32 y,
						std::vector<CObject*>& ObjectPtrs, direction_t facedir) :
CObject(pmap, x, y, OBJ_NONE),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_ObjectPtrs(ObjectPtrs),
m_climbing(false)
{
	m_index = 0;
	m_direction = facedir;
	m_ActionBaseOffset = 0x98C;
	setActionForce(A_KEEN_STAND);

	memset(m_playcontrol, 0,PA_MAX_ACTIONS);

	m_pfiring = false;
	m_jumpheight = 0;

	processActionRoutine();
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

	if(!m_climbing)
		processLooking();

	processFiring();

	processFalling();

	processExiting();

	moveXDir(xinertia);
	xinertia = 0;

	processActionRoutine();
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

		m_playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(m_index, IC_POGO) ? 1 : 0;
	}


	if(g_pInput->getTwoButtonFiring(m_index))
	{
		if(m_playcontrol[PA_JUMP] && m_playcontrol[PA_POGO])
		{
			m_playcontrol[PA_FIRE] = 1;
			m_playcontrol[PA_JUMP] = 0;
			m_playcontrol[PA_POGO] = 0;
		}
		/*else if(!m_playcontrol[PA_JUMP] || !m_playcontrol[PA_POGO])
		{
			m_playcontrol[PA_FIRE] = 0;
		}*/
	}
	else
	{
		m_playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(m_index, IC_FIRE)   ? 1 : 0;
	}
}

void CPlayerLevel::processFiring()
{
	bool inair = getActionNumber(A_KEEN_JUMP) || getActionNumber(A_KEEN_JUMP+1) ||
			getActionNumber(A_KEEN_FALL) || falling;

	bool shooting =  getActionNumber(A_KEEN_JUMP_SHOOT) || getActionNumber(A_KEEN_JUMP_SHOOTDOWN) ||
			getActionNumber(A_KEEN_JUMP_SHOOTUP) || getActionNumber(A_KEEN_SHOOT+2);

	if( m_playcontrol[PA_FIRE] && !shooting )
	{
		if( inair )
		{
			if(m_playcontrol[PA_Y] < 0)
				setAction(A_KEEN_JUMP_SHOOTUP);
			else if(m_playcontrol[PA_Y] > 0)
				setAction(A_KEEN_JUMP_SHOOTDOWN);
			else
				setAction(A_KEEN_JUMP_SHOOT);
		}
		else
		{
			if(m_playcontrol[PA_Y] < 0)
				setAction(A_KEEN_SHOOT+2);
			else
				setAction(A_KEEN_SHOOT);
		}
	}



}

void CPlayerLevel::processFalling()
{
	if(m_climbing) return;

	CObject::processFalling();

	if( falling && !getActionNumber(A_KEEN_JUMP_SHOOT)
			&& !getActionNumber(A_KEEN_JUMP_SHOOTUP) && !getActionNumber(A_KEEN_JUMP_SHOOTDOWN) )
		setAction(A_KEEN_FALL);
}

void CPlayerLevel::processMoving()
{
	size_t movespeed = 50;

	// Normal moving
	if(!m_playcontrol[PA_FIRE])
	{
		if( m_playcontrol[PA_X]<0 && !blockedl)
		{
			xinertia = -movespeed;
			m_direction = LEFT;
		}
		else if( m_playcontrol[PA_X]>0 && !blockedr)
		{
			xinertia = movespeed;
			m_direction = RIGHT;
		}

		// Now check if Player has the chance to climb a pole or something similar
		if(hitdetectWithTileProperty(1)) // 1 -> stands for pole Property
		{
			// Hit pole!
			// TODO: This code does not work yet correctly for some reason
			if( !m_climbing && m_playcontrol[PA_Y] != 0 )
			{
				m_climbing = true;
			}
			else if(m_climbing)
			{
				if(m_playcontrol[PA_Y] < 0)
				{
					if(!getActionNumber(A_KEEN_POLE_CLIMB))
						setAction(A_KEEN_POLE_CLIMB);
					//yinertia = -1;
				}
				else if(m_playcontrol[PA_Y] > 0)
				{
					//setAction(A_KEEN_POLE_SLIDE);
					//yinertia = 1;
				}
			}
		}
		else
		{
			m_climbing = false;
		}

	}

	if( blockedd )
	{
		if(xinertia != 0)
			setAction(A_KEEN_RUN);
		else if(m_playcontrol[PA_Y] == 0)
			setAction(A_KEEN_STAND);
	}
}

// Processes the jumping of the player
void CPlayerLevel::processJumping()
{
	if(!getActionNumber(A_KEEN_JUMP))
	{
		if(blockedd)
			m_jumpheight = 0;

		// Not jumping? Let's see if we can prepare the player to do so
		if(m_playcontrol[PA_JUMP] && (getActionNumber(A_KEEN_STAND) or getActionNumber(A_KEEN_RUN)) )
		{
			yinertia = -136;
			setAction(A_KEEN_JUMP);
			m_climbing = false;
		}
	}
	else
	{
		// while button is pressed, make the player jump higher
		if(m_playcontrol[PA_JUMP] || m_jumpheight <= MIN_JUMPHEIGHT)
			m_jumpheight++;
		else
			m_jumpheight = 0;

		// Set another jump animation if Keen is near yinertia == 0
		if( yinertia > -10 )
			setAction(A_KEEN_JUMP+1);

		// If the max. height is reached or the player cancels the jump by release the button
		// make keen fall
		if( m_jumpheight == 0 || m_jumpheight >= MAX_JUMPHEIGHT )
		{
			yinertia = 0;
			m_jumpheight = 0;
		}

	}
}

// This is for processing the looking routine.
void CPlayerLevel::processLooking()
{
	// Looking Up and Down Routine
	//bool notshooting = ;
	if(blockedd && xinertia == 0 /*&& notshooting*/)
	{
		if( m_playcontrol[PA_Y]<0 )
			setAction(A_KEEN_LOOKUP);
		else if( m_playcontrol[PA_Y]>0 )
			setAction(A_KEEN_LOOKDOWN);
		//else
			//setAction(A_KEEN_STAND);
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

}
