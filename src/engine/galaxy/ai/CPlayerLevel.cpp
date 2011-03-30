/*
 * CPlayerLevel.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CPlayerLevel.h"
#include "CBullets.h"
#include "CItemEffect.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CInput.h"
#include "sdl/sound/CSound.h"
#include "CVec.h"

namespace galaxy {

const Uint32 MAX_JUMPHEIGHT = 30;
const Uint32 MIN_JUMPHEIGHT = 10;

const Uint32 MAX_POGOHEIGHT = 20;
const Uint32 MIN_POGOHEIGHT = 5;

const int POGO_START_INERTIA = -100;
const int POGO_START_INERTIA_MAX = -173;
const int POGO_X_MAX_INERTIA = 55;
const int POGO_X_BOOST = 4;
const int POGO_START_INERTIA_IMPOSSIBLE = -200;

CPlayerLevel::CPlayerLevel(CMap *pmap, Uint32 x, Uint32 y,
						std::vector<CObject*>& ObjectPtrs, direction_t facedir,
						CInventory &l_Inventory) :
CObject(pmap, x, y, OBJ_PLAYER),
m_Inventory(l_Inventory),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_ObjectPtrs(ObjectPtrs),
m_cliff_hanging(false),
m_camera(pmap,x,y,this)
{
	m_index = 0;
	m_timer = 0;
	m_dying = false;
	m_hDir = facedir;
	m_ActionBaseOffset = 0x98C;
	m_ptogglingswitch = false;
	setActionForce(A_KEEN_STAND);

	memset(m_playcontrol, 0, PA_MAX_ACTIONS);

	m_pfiring = false;
	m_jumpheight = 0;
	m_climbing = false;
	m_inair = false;
	m_pogotoggle = false;
	m_jumped = false;

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

	if(m_dying)
	{
		processDying();
		return;
	}

	processInput();

	performCollisionsSameBox();

	if(supportedbyobject)
		blockedd = true;

	if(getActionNumber(A_KEEN_SLIDE))
	{
		processPlaceGem();
	}
	else
	{
		std::vector<CTileProperties> &TileProperty = g_pBehaviorEngine->getTileProperties();
		TileProperty[mp_Map->at(getXMidPos()>>CSF, getYMidPos()>>CSF)].bdown;

		if(!getActionNumber(A_KEEN_ENTER_DOOR) )
			processMoving();

		if(!m_cliff_hanging)
		{
			// Check whether we should bump the head
			if(!blockedd && blockedu && !m_BumpHead)
			{
				g_pSound->playSound( SOUND_KEEN_BUMPHEAD );
				m_BumpHead = true;
			}

			if(!blockedu)
				m_BumpHead = false;

			processJumping();
			processPogo();
			processFiring();

			if(!m_climbing)
			{
				if( getActionNumber(A_KEEN_ENTER_DOOR) && !getActionNumber(A_KEEN_POGO) )
				{
					processEnterDoor();
				}
				else
				{
					processFalling();
					processLooking();
					processExiting();
				}
			}
		}

		processLevelMiscFlagsCheck();
	}

	processActionRoutine();

	moveXDir(xinertia);

	if( !getActionNumber(A_KEEN_POGO) )
		xinertia = 0;

	m_camera.process();

	supportedbyobject = false;
}

void CPlayerLevel::processInput()
{
	// Entry for every player
	m_playcontrol[PA_X] = 0;
	m_playcontrol[PA_Y] = 0;

	if(g_pInput->getHoldedCommand(m_index, IC_LEFT))
		m_playcontrol[PA_X] -= 100;
	else if(g_pInput->getHoldedCommand(m_index, IC_RIGHT))
		m_playcontrol[PA_X] += 100;

	if(g_pInput->getHoldedCommand(m_index, IC_DOWN))
		m_playcontrol[PA_Y] += 100;
	else if(g_pInput->getHoldedCommand(m_index, IC_UP))
		m_playcontrol[PA_Y] -= 100;

	if(g_pInput->getHoldedCommand(m_index, IC_UPPERLEFT))
	{
		m_playcontrol[PA_X] -= 100;
		m_playcontrol[PA_Y] -= 100;
	}
	else if(g_pInput->getHoldedCommand(m_index, IC_UPPERRIGHT))
	{
		m_playcontrol[PA_X] += 100;
		m_playcontrol[PA_Y] -= 100;
	}
	else if(g_pInput->getHoldedCommand(m_index, IC_LOWERLEFT))
	{
		m_playcontrol[PA_X] -= 100;
		m_playcontrol[PA_Y] += 100;
	}
	else if(g_pInput->getHoldedCommand(m_index, IC_LOWERRIGHT))
	{
		m_playcontrol[PA_X] += 100;
		m_playcontrol[PA_Y] += 100;
	}

	if(!m_pfiring)
	{
		if(g_pInput->getHoldedCommand(m_index, IC_JUMP))
			m_playcontrol[PA_JUMP]++;
		else
			m_playcontrol[PA_JUMP] = 0;

	}
	else
		m_playcontrol[PA_JUMP]   = g_pInput->getHoldedCommand(m_index, IC_JUMP)   ? 1 : 0;

	m_playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(m_index, IC_POGO)   ? 1 : 0;

	// The possibility to charge jumps. This is mainly used for the pogo.
	if( m_playcontrol[PA_JUMP] > 50) m_playcontrol[PA_JUMP] = 50;

	// Two button firing process
	if(g_pInput->getTwoButtonFiring(m_index))
	{
		if(m_playcontrol[PA_JUMP] && m_playcontrol[PA_POGO])
		{
			m_playcontrol[PA_FIRE] = 1;
			m_playcontrol[PA_JUMP] = 0;
			m_playcontrol[PA_POGO] = 0;
		}
		else if(m_playcontrol[PA_FIRE])
		{
			m_playcontrol[PA_FIRE] = 0;
			m_playcontrol[PA_JUMP] = 0;
			m_playcontrol[PA_POGO] = 0;
			g_pInput->flushCommand(IC_JUMP);
			g_pInput->flushCommand(IC_FIRE);
			g_pInput->flushCommand(IC_POGO);
		}

	}
	else
		m_playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(m_index, IC_FIRE)   ? 1 : 0;
}

void CPlayerLevel::processFiring()
{
	if( m_playcontrol[PA_FIRE] && m_climbing )
		yinertia = 0;

	stItemGalaxy &m_Item = m_Inventory.Item;

	if( m_playcontrol[PA_FIRE] && !m_pfiring )
	{
		if(m_climbing)
		{
			if(m_playcontrol[PA_Y] < 0 && !getActionNumber(A_KEEN_POLE_SHOOTUP))
			{
				setAction(A_KEEN_POLE_SHOOTUP);
				const int newx = getXMidPos()-(3<<STC);
				const int newy = getYUpPos()-(16<<STC);
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, UP));
			}
			else if(m_playcontrol[PA_Y] > 0 && !getActionNumber(A_KEEN_POLE_SHOOTDOWN))
			{
				setAction(A_KEEN_POLE_SHOOTDOWN);
				const int newx = getXMidPos()-(3<<STC);
				const int newy = getYDownPos();
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, DOWN));
			}
			else if(!getActionNumber(A_KEEN_POLE_SHOOT))
			{
				setAction(A_KEEN_POLE_SHOOT);
				const int newx = getXPosition() + ((m_hDir == LEFT) ? -(16<<STC) : (16<<STC));
				const int newy = getYPosition()+(4<<STC);
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, m_hDir));
			}
			m_pfiring = true;
		}
		else if( m_inair )
		{
			if(m_playcontrol[PA_Y] < 0 && !getActionNumber(A_KEEN_JUMP_SHOOTUP))
			{
				setAction(A_KEEN_JUMP_SHOOTUP);
				const int newx = getXMidPos()-(3<<STC);
				const int newy = getYUpPos()-(16<<STC);
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, UP));

			}
			else if(m_playcontrol[PA_Y] > 0 && !getActionNumber(A_KEEN_JUMP_SHOOTDOWN))
			{
				setAction(A_KEEN_JUMP_SHOOTDOWN);
				const int newx = getXMidPos()-(3<<STC);
				const int newy = getYDownPos();
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, DOWN));

			}
			else if(!getActionNumber(A_KEEN_JUMP_SHOOT))
			{
				setAction(A_KEEN_JUMP_SHOOT);
				const int newx = getXPosition() + ((m_hDir == LEFT) ? -(16<<STC) : (16<<STC));
				const int newy = getYPosition()+(4<<STC);
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, m_hDir));
			}
			m_pfiring = true;
		}
		else
		{
			if(m_playcontrol[PA_Y] < 0)
			{
				setActionForce(A_KEEN_SHOOT+2);
				const int newx = getXMidPos()-(3<<STC);
				const int newy = getYUpPos()-(16<<STC);
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, UP));

				m_pfiring = true;
			}
			else if(!getActionNumber(A_KEEN_LOOKDOWN))
			{
				setAction(A_KEEN_SHOOT);
				const int newx = getXPosition() + ((m_hDir == LEFT) ? -(16<<STC) : (16<<STC));
				const int newy = getYPosition()+(4<<STC);
				if(m_Item.m_bullets > 0)
					m_ObjectPtrs.push_back(new CBullets(mp_Map, newx, newy, m_hDir));
				m_pfiring = true;
			}
		}

		// One shot less in the inventory when Keens shoots
		if(m_pfiring)
		{
			if(m_Item.m_bullets > 0)
			{
				g_pSound->playSound( SOUND_KEEN_FIRE );
				m_Item.m_bullets--;
			}
			else
			{
				g_pSound->playSound( SOUND_GUN_CLICK );
			}
		}
	}

	if( m_playcontrol[PA_FIRE] == 0 )
		m_pfiring = false;

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
				m_camera.m_freeze = false;
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
				m_camera.m_freeze = true;
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
			Uint32 l_x = ( getXLeftPos() + getXRightPos() ) / 2;
			Uint32 l_y_up = getYUpPos()-(7<<STC);
			Uint32 l_y_down = getYDownPos()-(7<<STC);
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
			else if(m_playcontrol[PA_Y] > 0 && !hitdetectWithTileProperty(1, l_x, l_y_down) )
			{
				m_climbing = false;
			}
			else if(m_playcontrol[PA_X] != 0)
			{
				m_climbing = false;
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
			if(!m_pfiring)
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
						if(!check_block && check_block_lower && m_inair && !getActionNumber(A_KEEN_POGO))
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
				else if( m_playcontrol[PA_X]>0 )
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
						if(!check_block && check_block_lower && m_inair && !getActionNumber(A_KEEN_POGO) )
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

				Uint32 l_x = ( getXLeftPos() + getXRightPos() ) / 2;
				Uint32 l_y_up = ( getYUpPos() );
				Uint32 l_y_down = ( getYDownPos() );
				// Now check if Player has the chance to climb a pole or something similar
				if( hitdetectWithTileProperty(1, l_x, l_y_up) || hitdetectWithTileProperty(1, l_x, l_y_down) ) // 1 -> stands for pole Property
				{
					// Hit pole!

					// calc the proper coord of that tile
					l_x = (l_x>>CSF)<<CSF;
					if( (!m_climbing && m_playcontrol[PA_Y] < 0 && hitdetectWithTileProperty(1, l_x, l_y_up)) ||
							( getActionNumber(A_KEEN_STAND) && m_playcontrol[PA_Y] > 0 && hitdetectWithTileProperty(1, l_x, l_y_down)) )
					{
						m_climbing = true;
						// Set Keen in climb mode
						setAction(A_KEEN_POLE);

						// Move to the proper X Coordinates, so Keen really grabs it!
						moveTo(VectorD2<int>(l_x - (7<<STC), getYPosition()));
						xinertia = 0;
					}
				}

				if( m_playcontrol[PA_Y]<0 )
				{
					// player pressed up
					if( !m_ptogglingswitch)
					{
						processPressUp();
						m_ptogglingswitch = true;
					}
				}
				else
					m_ptogglingswitch = false;

				// Check if Keen hits the floor
				if( blockedd && !m_cliff_hanging )
				{
					if(!getActionNumber(A_KEEN_POGO) )
					{
						if(moving != NONE)
						{
							setAction(A_KEEN_RUN);
							g_pSound->playSound( SOUND_KEEN_WALK );
						}
						else if(m_playcontrol[PA_Y] == 0)
							setAction(A_KEEN_STAND);
					}
				}
			}
		}
	}
}

// Processes the jumping of the player
void CPlayerLevel::processJumping()
{
	m_inair = getActionNumber(A_KEEN_JUMP) || getActionNumber(A_KEEN_JUMP+1) ||
			getActionNumber(A_KEEN_FALL) || falling;

	if( !m_inair && !m_playcontrol[PA_JUMP] )
		m_jumped = false;

	if(!getActionNumber(A_KEEN_JUMP))
	{
		if(blockedd)
			m_jumpheight = 0;

		// Not jumping? Let's see if we can prepare the player to do so
		if(m_playcontrol[PA_JUMP] && !m_jumped &&
				(getActionNumber(A_KEEN_STAND) ||
						getActionNumber(A_KEEN_RUN) ||
						getActionNumber(A_KEEN_POLE) ||
						getActionNumber(A_KEEN_POLE_CLIMB) ||
						getActionNumber(A_KEEN_POLE_SLIDE)) )
		{
			yinertia = -140;
			setAction(A_KEEN_JUMP);
			m_jumped = true;
			m_climbing = false;
			m_vDir = NONE;
			g_pSound->playSound( SOUND_KEEN_JUMP );
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

// Here all the pogo code is processed
void CPlayerLevel::processPogo()
{
	if(m_pfiring)
		return;

	if(!m_playcontrol[PA_POGO])
		m_pogotoggle = false;

	if(!getActionNumber(A_KEEN_POGO) && !m_pogotoggle)
	{
		if(blockedd)
			m_jumpheight = 0;

		// Not pogoing? Let's see if we can prepare the player to do so
		if( m_playcontrol[PA_POGO] )
		{
			if( getActionNumber(A_KEEN_STAND) || getActionNumber(A_KEEN_RUN) )
			{
				if(m_playcontrol[PA_JUMP])
					yinertia = POGO_START_INERTIA_IMPOSSIBLE;
				else
					yinertia = POGO_START_INERTIA;

				m_jumpheight = 0;
				setAction(A_KEEN_POGO);
				g_pSound->playSound( SOUND_KEEN_POGO );
				m_pogotoggle = true;
			}
			else if( getActionNumber(A_KEEN_FALL) || getActionNumber(A_KEEN_JUMP) )
			{
				m_jumpheight = MAX_POGOHEIGHT;
				setAction(A_KEEN_POGO);
				m_pogotoggle = true;
			}
		}
	}
	else
	{
		// while button is pressed, make the player jump higher
		if( (m_playcontrol[PA_JUMP] && m_jumpheight <= MAX_POGOHEIGHT) ||
				m_jumpheight <= MIN_POGOHEIGHT )
		{
			m_jumpheight++;
		}

		// pressed again will make keen fall until hitting the ground
		if(m_playcontrol[PA_POGO] && !m_pogotoggle)
		{
			m_jumpheight = 0;
			setAction(A_KEEN_FALL);
			m_pogotoggle = true;
		}

		// When keen hits the floor, start the same pogoinertia again!
		if(blockedd)
		{
			if(m_playcontrol[PA_JUMP])
				yinertia = POGO_START_INERTIA_MAX;
			else
				yinertia = POGO_START_INERTIA;

			m_jumpheight = 0;
			g_pSound->playSound( SOUND_KEEN_POGO );
		}

		if(m_playcontrol[PA_X] > 0)
		{
			if(xinertia <= POGO_X_MAX_INERTIA)
				xinertia += POGO_X_BOOST;
		}
		else if(m_playcontrol[PA_X] < 0)
		{
			if(xinertia >= -POGO_X_MAX_INERTIA)
				xinertia -= POGO_X_BOOST;
		}

		if( blockedr && xinertia > 0 )
			xinertia -= POGO_X_BOOST;
		else if( blockedl && xinertia < 0 )
			xinertia += POGO_X_BOOST;
	}
}

// Falling code
void CPlayerLevel::processFalling()
{
	CObject::processFalling();

	if( falling && !getActionNumber(A_KEEN_JUMP_SHOOT)
			&& !getActionNumber(A_KEEN_JUMP_SHOOTUP)
			&& !getActionNumber(A_KEEN_JUMP_SHOOTDOWN)
			&& !getActionNumber(A_KEEN_POGO) )
	{
		// TODO: This will need additional conditions to handle coming off of a pole.
		if ( getActionNumber(A_KEEN_RUN) )
		{
			g_pSound->playSound( SOUND_KEEN_FALL );
		}
		setAction(A_KEEN_FALL);
	}

	if(getActionNumber(A_KEEN_FALL))
		xinertia += (m_playcontrol[PA_X]>>1);
}


// This is for processing the looking routine.
void CPlayerLevel::processLooking()
{
	if(getActionNumber(A_KEEN_SHOOT+2))
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
	Uint32 x = getXMidPos();
	if( ((mp_Map->m_width-2)<<CSF) < x || (2<<CSF) > x )
	{
		CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
		EventContainer.add( new EventExitLevel(mp_Map->getLevel(), true) );
	}
}

#define		MISCFLAG_SWITCHPLATON 5
#define 	MISCFLAG_SWITCHPLATOFF 6
#define		MISCFLAG_SWITCHBRIDGE 15
#define		MISCFLAG_DOOR		2
#define		MISCFLAG_KEYCARDDOOR		32

int CPlayerLevel::processPressUp() {

	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);
	const int x_left = getXLeftPos();
	const int x_right = getXRightPos();
	const int x_mid = (x_left+x_right)/2;
	const int up_y = getYUpPos()+(3<<STC);

	const Uint32 tile_no = mp_Map->getPlaneDataAt(1, x_mid, up_y);
	int flag = Tile[tile_no].behaviour;

	/* pressing a switch */
	if (flag==MISCFLAG_SWITCHPLATON || flag == MISCFLAG_SWITCHPLATOFF ||
		flag == MISCFLAG_SWITCHBRIDGE)
	{
		g_pSound->playSound( SOUND_GUN_CLICK );
		setAction(A_KEEN_SLIDE);
		if(flag == MISCFLAG_SWITCHBRIDGE)
		{
			Uint32 newtile;
			if(Tile[tile_no+1].behaviour == MISCFLAG_SWITCHBRIDGE)
				newtile = tile_no+1;
			else
				newtile = tile_no-1;

			mp_Map->setTile( x_mid>>CSF, up_y>>CSF, newtile, true, 1); // Wrong tiles, those are for the info plane
			PressBridgeSwitch(x_mid, up_y);
		}
		else
		{
			const Uint32 newtile = (flag == MISCFLAG_SWITCHPLATON) ? tile_no+1 : tile_no-1 ;
			mp_Map->setTile( x_mid>>CSF, up_y>>CSF, newtile, true, 1); // Wrong tiles, those are for the info plane
			PressPlatformSwitch(x_mid, up_y);
		}
	 }
/*		var2 = o->boxTXmid*256-64;
		if (o->xpos == var2) {
			setAction(ACTION_KEENENTERSLIDE);
			setAction(A_KEEN_SLIDE);
		} else {
			o->time = var2;
			//setAction(ACTION_KEENPRESSSWITCH);
			setAction(ACTION_KEENENTERSLIDE);
		}
		EnterDoorAttempt = 1;
		return 1;
	} */

	flag = Tile[mp_Map->getPlaneDataAt(1, x_left, up_y)].behaviour;

	// entering a door
	if (flag == MISCFLAG_DOOR || flag == MISCFLAG_KEYCARDDOOR) {
		//int var2 = mid_x * 256+96;
		flag = Tile[mp_Map->getPlaneDataAt(1, x_right, up_y)].behaviour;
		//if (flag2 == MISCFLAG_DOOR || flag2 == MISCFLAG_KEYCARDDOOR) var2-=256;
		//if (getXPosition() == var2) {
		if(flag == MISCFLAG_DOOR || flag == MISCFLAG_KEYCARDDOOR) {
			/*if (flag == MISCFLAG_KEYCARDDOOR) {
				if (security_card) {
					security_card = 0;
					SD_PlaySound(SOUND_OPENSECURITYDOOR);
					GetNewObj(0);
					new_object->xpos = o->boxTXmid-2;
					new_object->ypos = o->boxTY2-4;
					new_object->active = 2;
					new_object->clipping = 0;
					new_object->type = 1;
					new_object->action = ACTION_SECURITYDOOROPEN;
					check_ground(new_object, ACTION_SECURITYDOOROPEN);
					o->action = ACTION_KEENENTERDOOR0;
					o->int16 = 0;
					return 1;
				} else {
					SD_PlaySound(SOUND_NOOPENSECURITYDOOR);
					o->action = ACTION_KEENSTAND;
					EnterDoorAttempt = 1;
					return 0;
				}
			} else {*/
				setAction(A_KEEN_ENTER_DOOR);
				//PlayLoopTimer = 110;
				//o->action = ACTION_KEENENTERDOOR1
				//o->int16 = 0;
				//if (*MAPSPOT(o->boxTXmid, o->boxTY1, INFOPLANE) == 0) sub_1FE94();
			//}
		}// else {
			//o->time = var2;
			//o->action = ACTION_KEENENTERSLIDE;
		//}
		//EnterDoorAttempt = 1;
		return 1;
	}
	return 0;
}

void CPlayerLevel::processEnterDoor()
{
	moveUp(16);

	if(!getActionStatus(A_KEEN_STAND))
		return;

	setAction(A_KEEN_STAND);

	int xmid = getXMidPos();
	int y1 = getYMidPos();

	Uint32 t = mp_Map->getPlaneDataAt(2, xmid, y1);
	if (t == 0) {
		//level_state = 13;
		//o->action = ACTION_KEENENTEREDDOOR;
		// TODO: Figure out what this does
		return;
	}

	if (t == 0xB1B1) {
		//level_state = 2;
		//o->action = ACTION_KEENENTEREDDOOR;
		// TODO: Figure out what this does
		return;
	}

	//int xpos = ((t%256 - 1)>>CSF) + 15;
	int ypos = ((t%256 - 1))<<CSF;
	//int ypos = (t >> 8 << 8);
	int xpos = (t >> 8)<<CSF;

	moveToForce(xpos, ypos);

	//o->ypos = TILE2MU(*t%256 - 1) + 15;
	//o->xpos = (*t >> 8 << 8);
	//o->int16 = 1;
	//o->clipping = 0;
	//set_sprite_action(o, (o->action)->next);
	//o->clipping = 1;
	//sub_183F1(o);
	return;

}

// Process the touching of certain tile, like items and hazards...
void CPlayerLevel::processLevelMiscFlagsCheck()
{
	// TODO: Here it a lot we have to do still.
	// Item which are taken must go into a data structure
	// animation should also be triggered

	stItemGalaxy &m_Item = m_Inventory.Item;

	// This will change the gemholder to a holder with gem
	if( getActionNumber(A_KEEN_STAND) || getActionNumber(A_KEEN_RUN) )
	{
		for( Uint32 i=7 ; i<=10 ; i++ )
		{
			const int l_x = getXMidPos();
			const int l_y = getYDownPos()-(3<<STC);

			if( hitdetectWithTileProperty(i, l_x, l_y) )
			{
				if(i == 7 && m_Item.m_gem.red > 0)
					m_Item.m_gem.red--;
				else if(i == 8 && m_Item.m_gem.yellow > 0)
					m_Item.m_gem.yellow--;
				else if(i == 9 && m_Item.m_gem.blue > 0)
					m_Item.m_gem.blue--;
				else if(i == 10 && m_Item.m_gem.green > 0)
					m_Item.m_gem.green--;
				else
					break;

				moveToHorizontal((l_x>>CSF)<<CSF);
				setAction(A_KEEN_SLIDE);
				g_pSound->playSound( SOUND_DOOR_OPEN );
			}
		}
	}

	int l_x = getXLeftPos();
	int l_y = getYUpPos();
	int l_w = getXRightPos() - getXLeftPos();
	int l_h = getYDownPos() - getYUpPos();

	// Deadly hazards! Here Keen dying routine will be triggered
	if(hitdetectWithTilePropertyRect(3, l_x, l_y, l_w, l_h, 1<<STC))
		kill();

	if(hitdetectWithTilePropertyRect(4, l_x, l_y, l_w, l_h, 1<<STC))
	{
		const int lc_x = l_x>>CSF;
		const int lc_y = l_y>>CSF;
		mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
		m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, 215, ANIMATE));
		m_Item.m_drops++;

		if(m_Item.m_drops==100)
		{
			m_Item.m_drops = 0;
			m_Item.m_lifes++;
			g_pSound->playSound( SOUND_EXTRA_LIFE );
			m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[10]));

		}
		g_pSound->playSound( SOUND_GET_DROP );
	}

	/// Tile Items (Sprite-Items are handled in the CSpriteItem Class)
	// All the collectable items go from 21 to 28
	for( Uint32 i=21 ; i<=28 ; i++ )
	{
		if(hitdetectWithTilePropertyRect(i, l_x, l_y, l_w, l_h, 1<<STC))
		{
			const int lc_x = l_x>>CSF;
			const int lc_y = l_y>>CSF;
			mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
			m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[4+i-21]));

			switch(i)
			{
			case 21: m_Item.m_points += 100;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 22: m_Item.m_points += 200;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 23: m_Item.m_points += 500;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 24: m_Item.m_points += 1000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 25: m_Item.m_points += 2000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 26: m_Item.m_points += 5000;	g_pSound->playSound( SOUND_GET_BONUS );	break;
			case 27: m_Item.m_lifes++;	g_pSound->playSound( SOUND_EXTRA_LIFE );	break;
			case 28: m_Item.m_bullets += 5;	g_pSound->playSound( SOUND_GET_AMMO );	break;
			default: break;
			}
		}
	}
}

/**
 * This function checks whether a bridge must be opened or closed and does this kind of work
 * I'm not really happy with that part of the code and I know that it works for Keen 4. Not sure about the
 * other episodes, but it's well though and should...
 */

/**
 * This function will open/close bridges in Keen Galaxy
 * \param lx CSFed Coordinate where the switch has been triggered
 */
void CPlayerLevel::PressBridgeSwitch(const Uint32 lx, const Uint32 ly)
{
	Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);

	Uint32 newX = targetXY >> 8;
	Uint32 newY = targetXY & 0xFF;

	const Uint32 start_tile = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF)-1;
	const Uint32 end_tile = start_tile+3;

	/// We found the start of the row, that need to be changed.
	/// Let apply it to the rest of the bridge
	// Apply to the borders

	// bridge opened or closed?
	const bool b_opened = (start_tile%8 < 4) ?true : false;

	int x = newX;
	for(int t = start_tile ;  ; x++ )
	{
		// Now decide whether the tile is a piece or borders of the bridge
		const Uint32 type = t%18;

		if(type < 16)
		{
			if(b_opened)
				t += 4;
			else
				t -= 4;
		}
		else
		{
			// It is just a normal piece remove
			t = (t/18)*18;
			if(b_opened)
				t+=16;
			else
				t+=17;
		}
		const Uint32 NewTile = t;
		t = mp_Map->getPlaneDataAt(1, x<<CSF, newY<<CSF);

		mp_Map->setTile(x-1, newY, NewTile, true, 1);
		mp_Map->setTile(x-1, newY+1, NewTile+18, true, 1);

		if(t == end_tile)
		{
			if(t%8 < 4)
				// This bridge is opened, close it!
				t += 4;
			else
				// This bridge is closed, open it!
				t -= 4;

			Uint32 new_lasttile = end_tile;
			if(b_opened)
				new_lasttile += 4;
			else
				new_lasttile -= 4;

			mp_Map->setTile(x-1, newY+1, new_lasttile+17, true, 1);
			mp_Map->setTile(x, newY, new_lasttile, true, 1);
			mp_Map->setTile(x, newY+1, new_lasttile+18, true, 1);
			break;
		}
	}

	return;
}

/**
 * This function will put/release the blockers of some platforms used in Keen Galaxy
 */
void CPlayerLevel::PressPlatformSwitch(const Uint32 lx, const Uint32 ly)
{
	Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);

	Uint32 newX = targetXY >> 8;
	Uint32 newY = targetXY & 0xFF;

	if(mp_Map->getPlaneDataAt(2, newX<<CSF, newY<<CSF) == 31)
		mp_Map->setTile(newX, newY, 0, true, 2);
	else
		mp_Map->setTile(newX, newY, 31, true, 2);

	return;
}

void CPlayerLevel::openDoorsTile()
{
	int lx = getXMidPos();
	int ly = getYDownPos()-(3<<STC);
	Uint32 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);

	Uint32 newX = targetXY >> 8;
	Uint32 newY = targetXY & 0xFF;
	Uint32 tileno, next_tileno;

	do
	{
		tileno = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF);
		mp_Map->setTile(newX, newY, tileno+1, true, 1);
		newY++;
		next_tileno = mp_Map->getPlaneDataAt(1, newX<<CSF, newY<<CSF);
	}while(next_tileno == tileno || next_tileno == tileno+18 || next_tileno == tileno+2*18);
}

void CPlayerLevel::processPlaceGem()
{
	if(m_timer == 0 || m_timer == 5 || m_timer == 8)
	{
		openDoorsTile();
	}

	if(m_timer < 10)
	{
		m_timer++;
		return;
	}

	m_timer = 0;

	setAction(A_KEEN_STAND);

	int lx = getXMidPos();
	int ly = getYDownPos()-(3<<STC);

	Uint32 tileno = mp_Map->getPlaneDataAt(1, lx, ly);
	mp_Map->setTile(lx>>CSF, ly>>CSF, tileno+18, true, 1);
}

void CPlayerLevel::processDying()
{
	// TODO: Here Keen must be falling out the screen, die effect of Keen Galaxy

	m_Inventory.Item.m_lifes--;
	setActionForce(A_KEEN_DIE);
	g_pSound->playSound( SOUND_KEEN_DIE );

	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	EventContainer.add( new EventExitLevel(mp_Map->getLevel(), false) );
}

void CPlayerLevel::kill()
{
	// TODO: Here were prepare Keen to die, setting that action
	m_dying = true;
}

}
