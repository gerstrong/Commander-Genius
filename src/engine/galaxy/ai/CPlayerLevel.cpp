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
m_cliff_hanging(false)
{
	m_index = 0;
	m_hDir = facedir;
	m_ActionBaseOffset = 0x98C;
	setActionForce(A_KEEN_STAND);

	memset(m_playcontrol, 0, PA_MAX_ACTIONS);

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

	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
	TileProperty[mp_Map->at(getXMidPos()>>CSF, getYMidPos()>>CSF)].bdown;

	processMoving();

	processJumping();

	if(!m_climbing)
		processLooking();

	processFiring();

	processFalling();

	processExiting();

	processActionRoutine();

	moveXDir(xinertia);
	xinertia = 0;
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
		m_playcontrol[PA_FIRE] = g_pInput->getHoldedCommand(m_index, IC_FIRE)   ? 1 : 0;
	}
}

void CPlayerLevel::processFiring()
{
	if(m_cliff_hanging)
		return;

	bool shooting =  getActionNumber(A_KEEN_JUMP_SHOOT) || getActionNumber(A_KEEN_JUMP_SHOOTDOWN) ||
			getActionNumber(A_KEEN_JUMP_SHOOTUP) || getActionNumber(A_KEEN_SHOOT+2) ||
			getActionNumber(A_KEEN_POLE_SHOOTUP) || getActionNumber(A_KEEN_POLE_SHOOTDOWN) ||
			getActionNumber(A_KEEN_POLE_SHOOT);

	if( m_playcontrol[PA_FIRE] && m_climbing )
		yinertia = 0;

	if( m_playcontrol[PA_FIRE] && !shooting )
	{
		if(m_climbing)
		{
			if(m_playcontrol[PA_Y] < 0 && !getActionNumber(A_KEEN_POLE_SHOOTUP))
				setAction(A_KEEN_POLE_SHOOTUP);
			else if(m_playcontrol[PA_Y] > 0 && !getActionNumber(A_KEEN_POLE_SHOOTDOWN))
				setAction(A_KEEN_POLE_SHOOTDOWN);
			else if(!getActionNumber(A_KEEN_POLE_SHOOT))
				setAction(A_KEEN_POLE_SHOOT);
		}
		else if( m_inair )
		{
			if(m_playcontrol[PA_Y] < 0 && !getActionNumber(A_KEEN_JUMP_SHOOTUP))
				setAction(A_KEEN_JUMP_SHOOTUP);
			else if(m_playcontrol[PA_Y] > 0 && !getActionNumber(A_KEEN_JUMP_SHOOTDOWN))
				setAction(A_KEEN_JUMP_SHOOTDOWN);
			else if(!getActionNumber(A_KEEN_JUMP_SHOOT))
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

void CPlayerLevel::processMoving()
{
	direction_t moving = NONE;
	std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();

	if(m_cliff_hanging)
	{
		if(getActionNumber(A_KEEN_CLIMB))
		{
			int dy = 32;
			int dx = dy/2;
			moveUp(dy);
			moveXDir( (m_hDir == LEFT) ? -dx : dx, true);
			if(getActionStatus(A_KEEN_STAND))
			{
				m_cliff_hanging = false;
				setAction(A_KEEN_STAND);
				setActionSprite();
				calcBouncingBoxes();
				moveDown(16*dy);
			}
		}
		else
		{
			if(m_playcontrol[PA_Y] < 0)
			{
				setAction(A_KEEN_CLIMB);
			}
			else if(m_playcontrol[PA_Y] > 0)
			{
				m_cliff_hanging = false;
				setAction(A_KEEN_FALL);
				setActionSprite();
				calcBouncingBoxes();
			}
			yinertia = 0;
		}
	}
	else
	{
		if(m_climbing)
		{
			// The climbing section for Keen
			Uint16 l_x = ( getXLeftPos() + getXRightPos() ) / 2;
			Uint16 l_y_up = getYUpPos()-(7<<STC);
			Uint16 l_y_down = getYDownPos()-(7<<STC);
			if(m_playcontrol[PA_Y] < 0 && hitdetectWithTileProperty(1, l_x, l_y_up) )
			{
				setAction(A_KEEN_POLE_CLIMB);
				m_vDir = UP;
			}
			else if(m_playcontrol[PA_Y] > 0 && hitdetectWithTileProperty(1, l_x, l_y_down) )
			{
				setAction(A_KEEN_POLE_SLIDE);
				m_vDir = DOWN;
			}
			else // == 0
			{
				setAction(A_KEEN_POLE);
				m_vDir = NONE;
			}
		}
		else
		{
			// Normal moving
			if(!m_playcontrol[PA_FIRE])
			{
				if( m_playcontrol[PA_X]<0 )
				{
					if(!blockedl)
					{
						// prepare him to walk
						moving = m_hDir = LEFT;
					}
					else
					{
						bool check_block = TileProperty[mp_Map->at((getXLeftPos()>>CSF)-1, getYUpPos()>>CSF)].bright;
						bool check_block_lower = TileProperty[mp_Map->at((getXLeftPos()>>CSF)-1, (getYUpPos()>>CSF)+1)].bright;
						if(!check_block && check_block_lower && m_inair)
						{
							setAction(A_KEEN_HANG);
							setActionSprite();
							calcBouncingBoxes();
							Uint32 x = (getXPosition()>>CSF)<<CSF;
							Uint32 y = ((getYPosition()>>CSF)+1)<<CSF;
							moveTo(x,y);
							m_cliff_hanging = true;
						}
					}
				}
				else if( m_playcontrol[PA_X]>0)
				{
					if(!blockedr)
					{
						// prepare him to walk
						moving = m_hDir = RIGHT;
					}
					else
					{
						bool check_block = TileProperty[mp_Map->at((getXRightPos()>>CSF)+1, getYUpPos()>>CSF)].bleft;
						bool check_block_lower = TileProperty[mp_Map->at((getXRightPos()>>CSF)+1, (getYUpPos()>>CSF)+1)].bleft;
						if(!check_block && check_block_lower && m_inair)
						{
							setAction(A_KEEN_HANG);
							setActionSprite();
							calcBouncingBoxes();
							Uint32 x = ((getXPosition()>>CSF)+1)<<CSF;
							Uint32 y = ((getYPosition()>>CSF)+1)<<CSF;
							moveTo(x,y);
							m_cliff_hanging = true;
						}
					}
				}

				Uint16 l_x = ( getXLeftPos() + getXRightPos() ) / 2;
				Uint16 l_y = ( getYUpPos() + getYDownPos() ) / 2;
				// Now check if Player has the chance to climb a pole or something similar
				if( hitdetectWithTileProperty(1, l_x, l_y) ) // 1 -> stands for pole Property
				{
					// Hit pole!

					// calc the proper coord of that tile
					l_x = (l_x>>CSF)<<CSF;
					if( !m_climbing && m_playcontrol[PA_Y] != 0 )
					{
						m_climbing = true;
						// Set Keen in climb mode
						setAction(A_KEEN_POLE);

						// Move to the proper X Coordinates, so Keen really grabs it!
						moveTo(VectorD2<int>(l_x - (7<<STC), getYPosition()));
						xinertia = 0;
					}
				}
			}

			if( blockedd && !m_cliff_hanging )
			{
				if(moving != NONE)
					setAction(A_KEEN_RUN);
				else if(m_playcontrol[PA_Y] == 0)
					setAction(A_KEEN_STAND);
			}
		}
	}
}

// Processes the jumping of the player
void CPlayerLevel::processJumping()
{
	if(m_cliff_hanging)
		return;

	m_inair = getActionNumber(A_KEEN_JUMP) || getActionNumber(A_KEEN_JUMP+1) ||
			getActionNumber(A_KEEN_FALL) || falling;

	if(!getActionNumber(A_KEEN_JUMP))
	{
		if(blockedd)
			m_jumpheight = 0;

		// Not jumping? Let's see if we can prepare the player to do so
		if(m_playcontrol[PA_JUMP] and
				(getActionNumber(A_KEEN_STAND) or
						getActionNumber(A_KEEN_RUN) or
						getActionNumber(A_KEEN_POLE) or
						getActionNumber(A_KEEN_POLE_CLIMB) or
						getActionNumber(A_KEEN_POLE_SLIDE)) )
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

		xinertia += (m_playcontrol[PA_X]>>1);
	}
}

// Falling code
void CPlayerLevel::processFalling()
{
	if(m_climbing || m_cliff_hanging) return;

	CObject::processFalling();

	if( falling && !getActionNumber(A_KEEN_JUMP_SHOOT)
			&& !getActionNumber(A_KEEN_JUMP_SHOOTUP) && !getActionNumber(A_KEEN_JUMP_SHOOTDOWN) )
		setAction(A_KEEN_FALL);

	if(getActionNumber(A_KEEN_FALL))
		xinertia += (m_playcontrol[PA_X]>>1);
}


// This is for processing the looking routine.
void CPlayerLevel::processLooking()
{
	if(m_cliff_hanging)
		return;

	// Looking Up and Down Routine
	if(blockedd && xinertia == 0 )
	{
		if( m_playcontrol[PA_Y]<0 )
			setAction(A_KEEN_LOOKUP);
		else if( m_playcontrol[PA_Y]>0 )
			setAction(A_KEEN_LOOKDOWN);
	}
}

// Processes the exiting of the player. Here all cases are held
void CPlayerLevel::processExiting()
{
	if(m_climbing || m_cliff_hanging)
		return;

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	Uint32 x = getXMidPos();
	if( ((mp_Map->m_width-2)<<CSF) < x || (2<<CSF) > x )
	{
		EventContainer.add( new EventExitLevel(mp_Map->getLevel()) );
	}
}

}
