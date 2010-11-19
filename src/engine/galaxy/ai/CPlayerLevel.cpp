/*
 * CPlayerLevel.cpp
 *
 *  Created on: 06.08.2010
 *      Author: gerstrong
 */

#include "CPlayerLevel.h"
#include "CItemEffect.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CInput.h"
#include "CVec.h"

// TODO: Needs recoding. This was just for testing, but now we need something much more
// serious. I want to create a diagram to represent all actions that keen can do and code them
// here according to that diagram. Something like UML or Petri. This Code got a bit out of control

namespace galaxy {

const Uint16 MAX_JUMPHEIGHT = 30;
const Uint16 MIN_JUMPHEIGHT = 10;

const Uint16 MAX_POGOHEIGHT = 20;
const Uint16 MIN_POGOHEIGHT = 5;

const int POGO_START_INERTIA = -100;
const int POGO_START_INERTIA_MAX = -170;
const int POGO_X_MAX_INERTIA = 55;
const int POGO_X_BOOST = 4;
const int POGO_START_INERTIA_IMPOSSIBLE = -200;

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
	m_timer = 0;
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
			processJumping();
			processPogo();
			processFiring();

			if(!m_climbing)
			{
				if(getActionNumber(A_KEEN_ENTER_DOOR))
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
			if(!getActionNumber(A_KEEN_POGO))
			{
				m_playcontrol[PA_JUMP]++;

				if(m_jumpheight >= (MAX_JUMPHEIGHT-2))
				{
					m_playcontrol[PA_JUMP] = 0;
					g_pInput->flushCommand(m_index, IC_JUMP);
				}
			}
			else
			{
				m_playcontrol[PA_JUMP] = 1;
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
					if( (!m_climbing && m_playcontrol[PA_Y] < 0) ||
							( getActionNumber(A_KEEN_STAND) && m_playcontrol[PA_Y] > 0 ) )
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
					processPressUp();
				}
			}

			// Check if Keen hits the floor
			if( blockedd && !m_cliff_hanging )
			{
				if(!getActionNumber(A_KEEN_POGO) )
				{
					if(moving != NONE)
						setAction(A_KEEN_RUN);
					else if(m_playcontrol[PA_Y] == 0)
						setAction(A_KEEN_STAND);
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
			m_vDir = NONE;
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
	if(!getActionNumber(A_KEEN_POGO))
	{
		if(blockedd)
			m_jumpheight = 0;

		// Not pogoing? Let's see if we can prepare the player to do so
		if( g_pInput->getPressedCommand(IC_POGO) )
		{
			if( getActionNumber(A_KEEN_STAND) or getActionNumber(A_KEEN_RUN) )
			{
				if(g_pInput->getHoldedCommand(IC_JUMP))
					yinertia = POGO_START_INERTIA_IMPOSSIBLE;
				else
					yinertia = POGO_START_INERTIA;

				m_jumpheight = 0;
				setAction(A_KEEN_POGO);
			}
			else if( getActionNumber(A_KEEN_FALL) or getActionNumber(A_KEEN_JUMP) )
			{
				m_jumpheight = MAX_POGOHEIGHT;
				setAction(A_KEEN_POGO);
			}
		}
	}
	else
	{
		// while button is pressed, make the player jump higher
		if( (g_pInput->getHoldedCommand(IC_JUMP) and m_jumpheight <= MAX_POGOHEIGHT) or
				m_jumpheight <= MIN_POGOHEIGHT )
		{
			m_jumpheight++;
		}

		// pressed again will make keen fall until hitting the ground
		if(g_pInput->getPressedCommand(IC_POGO))
		{
			m_jumpheight = 0;
			setAction(A_KEEN_FALL);
		}

		// When keen hits the floor, start the same pogoinertia again!
		if(blockedd)
		{
			if(g_pInput->getHoldedCommand(IC_JUMP))
				yinertia = POGO_START_INERTIA_MAX;
			else
				yinertia = POGO_START_INERTIA;
			m_jumpheight = 0;
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
		setAction(A_KEEN_FALL);

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
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

	Uint32 x = getXMidPos();
	if( ((mp_Map->m_width-2)<<CSF) < x || (2<<CSF) > x )
	{
		EventContainer.add( new EventExitLevel(mp_Map->getLevel()) );
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
	const int up_y = getYUpPos();

	int flag = Tile[mp_Map->getPlaneDataAt(1, x_left, up_y)].behaviour;

	/* pressing a switch */
	/*if (flag==MISCFLAG_SWITCHPLATON||flag==MISCFLAG_SWITCHPLATOFF||
	 flag == MISCFLAG_SWITCHBRIDGE) {
		var2 = o->boxTXmid*256-64;
		if (o->xpos == var2) {
			o->action = ACTION_KEENENTERSLIDE;
		} else {
			o->time = var2;
			o->action = ACTION_KEENPRESSSWITCH;
		}
		EnterDoorAttempt = 1;
		return 1;
	} */

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

	Uint16 t = mp_Map->getPlaneDataAt(2, xmid, y1);
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

// Process the item collecting
void CPlayerLevel::processLevelMiscFlagsCheck()
{
	// TODO: Here it a lot we have to do still.
	// Item which are taken must go into a data structure
	// animation should also be triggered

	// This will change the gemholder to a holder with gem
	if( getActionNumber(A_KEEN_STAND) || getActionNumber(A_KEEN_RUN) )
	{
		for( Uint16 i=7 ; i<=10 ; i++ )
		{
			int l_x = getXMidPos();
			int l_y = getYDownPos()-(3<<STC);

			if( hitdetectWithTileProperty(i, l_x, l_y) )
			{
				setAction(A_KEEN_SLIDE);
			}
		}
	}

	int l_x = getXLeftPos();
	int l_y = getYUpPos();
	int l_w = getXRightPos() - getXLeftPos();
	int l_h = getYDownPos() - getYUpPos();
	// Check for the items
	if(hitdetectWithTilePropertyRect(4, l_x, l_y, l_w, l_h, 1<<STC))
	{
		const int lc_x = l_x>>CSF;
		const int lc_y = l_y>>CSF;
		mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
		m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, 215, ANIMATE));
	}

	// All the collectable items go from 21 to 28
	for( Uint16 i=21 ; i<=28 ; i++ )
	{
		if(hitdetectWithTilePropertyRect(i, l_x, l_y, l_w, l_h, 1<<STC))
		{
			const int lc_x = l_x>>CSF;
			const int lc_y = l_y>>CSF;
			mp_Map->setTile( lc_x, lc_y, 0, true, 1 );
			m_ObjectPtrs.push_back(new CItemEffect(mp_Map, lc_x<<CSF, lc_y<<CSF, got_sprite_item_pics[4+i-21]));
		}
	}
}

void CPlayerLevel::openDoorsTile()
{
	int lx = getXMidPos();
	int ly = getYDownPos()-(3<<STC);
	Uint16 targetXY = mp_Map->getPlaneDataAt(2, lx, ly);

	Uint16 newX = targetXY >> 8;
	Uint16 newY = targetXY & 0xFF;
	Uint16 tileno, next_tileno;

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

		//tile = MAPSPOTNP(o->boxTXmid, o->boxTY2)

		//{

		//new_fg_tile = *MK_FP(mapsegs[FGPLANE],tile+18);
		//targetXY = *MK_FP(mapsegs[INFOPLANE], tile);


		/*RF_11(&new_fg_tile, 1, o->boxTXmid, o->boxTY2, 1, 1);
		SD_PlaySound(SOUND_OPENGEMDOOR);
		GetNewObj(0);

		new_object->xpos = newX;
		new_object->ypos = newY;

		if (newX > map_width_T || newX < 2 ||
		 newY > map_heightT || newY < 2) {
			Quit("Keyholder points to a bad spot!");
		//}

		tile_pointer = MAPSPOT(newX, newY, FGPLANE);
		foreground_tile = *tile_pointer;

		height = 1;
		tile_pointer += map_width_T;

		while(*tile_pointer == foreground_tile) {
			height++;
			tile_pointer += map_width_T;
		};

		new_object->time = height;
		new_object->active = 2;
		new_object->clipping = 0;
		new_objective->type = 1;
		CheckGround(new_object, ACTION_DOOROPEN);
		return;*/
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

	Uint16 tileno = mp_Map->getPlaneDataAt(1, lx, ly);
	mp_Map->setTile(lx>>CSF, ly>>CSF, tileno+18, true, 1);
}


}
