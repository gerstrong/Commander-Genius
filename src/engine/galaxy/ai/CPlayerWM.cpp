/*
 * CPlayerWM.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
 *
 *  The code for the player when he is shown on the map...
 */

#include "CPlayerWM.h"
#include "engine/galaxy/ai/CFlag.h"
#include "common/CBehaviorEngine.h"
#include "sdl/CInput.h"
#include "sdl/sound/CSound.h"
#include "CVec.h"

const Uint16 WALKBASEFRAME = 130;
const Uint16 SWIMBASEFRAME = 156;

namespace galaxy {

CPlayerWM::CPlayerWM(CMap *pmap,
		Uint32 x,
		Uint32 y,
		std::vector<CObject*>& ObjectPtrs,
		CInventory &l_Inventory,
		stCheat &Cheatmode):
CPlayerBase(pmap, x, y,
		    ObjectPtrs,
		    LEFT,
		    l_Inventory,
		    Cheatmode),
m_basesprite(WALKBASEFRAME),
m_looking_dir(LEFT),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_cantswim(false)
{
	sprite = m_basesprite;
	performCollisions();
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

	processMoving();

	// Events for the Player are processed here.
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	if( EventPlayerEndLevel* ev = EventContainer.occurredEvent<EventPlayerEndLevel>() )
	{
		if(ev->sucess)
		{
			finishLevel(ev->levelObject);
		}
		EventContainer.pop_Event();
	}

	m_camera.process();
	m_camera.processEvents();
}

/*
 * Processes the walking of the player on map here
 */
void CPlayerWM::processMoving()
{
	// Check if the player is swimming or walking and setup the proper speed
	int movespeed;
	if(m_basesprite == SWIMBASEFRAME)
		movespeed = 25;
	else if(m_basesprite == WALKBASEFRAME)
		movespeed = 50;
	else
		movespeed = 0;

	bool walking=false;

	bool bleft, bright, bup, bdown;

	// This will trigger between swim and walkmode
	checkforSwimming(bleft, bright, bup, bdown);

	// Normal walking
	if(g_pInput->getHoldedCommand(IC_LEFT) && !bleft)
	{
		if(!g_pInput->getHoldedCommand(IC_UP) && !g_pInput->getHoldedCommand(IC_DOWN))
			m_vDir = NONE;

		moveLeft(movespeed);
		walking = true;
		m_hDir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT) && !bright)
	{
		if(!g_pInput->getHoldedCommand(IC_UP) && !g_pInput->getHoldedCommand(IC_DOWN))
			m_vDir = NONE;

		moveRight(movespeed);
		walking = true;
		m_hDir = RIGHT;
	}

	if(g_pInput->getHoldedCommand(IC_UP) && !bup)
	{
		if(!g_pInput->getHoldedCommand(IC_LEFT) && !g_pInput->getHoldedCommand(IC_RIGHT))
			m_hDir = NONE;

		moveUp(movespeed);
		walking = true;
		m_vDir = UP;
	}
	else if(g_pInput->getHoldedCommand(IC_DOWN) && !bdown)
	{
		if(!g_pInput->getHoldedCommand(IC_LEFT) && !g_pInput->getHoldedCommand(IC_RIGHT))
			m_hDir = NONE;

		moveDown(movespeed);
		walking = true;
		m_vDir = DOWN;
	}

	// In case noclipping was triggered, make it solid, or remove it...
	if(m_Cheatmode.noclipping)
	{
		solid = !solid;
		m_Cheatmode.noclipping = false;
	}

	// perform actions depending on if the jump button was pressed
	if(g_pInput->getPressedCommand(IC_JUMP))
	{
		// Get the object
		Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
		if(object) // if we found an object
		{
			// start the level
			startLevel(object);
			g_pInput->flushCommands();
		}
	}

	// If keen is just walking on the map or swimming in the sea. Do the proper animation for it.
	if(m_basesprite == WALKBASEFRAME)
	{
		performWalkingAnimation(walking);
		m_cantswim = false;
	}
	else if(m_basesprite == SWIMBASEFRAME)
	{
		if(m_Inventory.Item.m_special.ep4.swimsuit)
		{
			performSwimmingAnimation();
		}
		else
		{
			if( !m_cantswim )
			{
				CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;

				g_pSound->playSound( SOUND_CANT_SWIM, PLAY_PAUSEALL );
				EventContainer.add( new EventSendBitmapDialogMsg(106,
						g_pBehaviorEngine->getString("CANT_SWIM_TEXT"), LEFT) );

				m_cantswim = true;
			}

		}
	}
}

/**
 * This function will help starting the level for Commander Keen
 */
void CPlayerWM::startLevel(Uint16 object)
{
	g_pBehaviorEngine->m_EventList.add(new EventEnterLevel(object));
}

/*
 *	makes the player finish the level
 */
void CPlayerWM::finishLevel(Uint16 object)
{
	// if a door or other blocker was found remove it
	int x, y;
	Uint16 door = object + 0xD000;
	while(mp_Map->findTile(door, &x, &y, 2))
	{
		// Open blocks in case there are
		mp_Map->setTile( x, y, 0, true, 1);
		mp_Map->setTile( x, y, 0, true, 2);
		mp_Map->redrawAt( x, y);
	}

	Uint16 flag_dest = object + 0xF000;
	if(mp_Map->findTile(flag_dest, &x, &y, 2))
	{
		// spawn the flag
		VectorD2<Uint32> src(getXPosition(), getYPosition());
		VectorD2<Uint32> dst((x<<CSF), (y<<CSF));

		CFlag *pFlag = new CFlag(mp_Map, src, dst);
		m_ObjectPtrs.push_back(pFlag);
		g_pSound->playSound( SOUND_FLAG_APPEAR );

		// Mark the tileinfo on the map as marked!!
		mp_Map->setTile( x, y, 0, true, 2);
	}
	g_pSound->playSound( SOUND_LEVEL_DONE );
}

/**
 * This is the function will switch between swim and walk mode
 * Those are the tileproperties to check for
 * 11      Enter water from top	Keen 4
 * 12      Enter water from right	Keen 4
 * 13      Enter water from bottom Keen 4
 * 14      Enter water from left	Keen 4
 */
void CPlayerWM::checkforSwimming(bool &bleft, bool &bright, bool &bup, bool &bdown)
{
	Uint16 left, right, up, down;
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);

	bleft = bright = bup = bdown = false;

	left = Tile[mp_Map->at( getXLeftPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
	right = Tile[mp_Map->at( getXRightPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
	up = Tile[mp_Map->at( getXMidPos()>>CSF, getYUpPos()>>CSF, 1)].behaviour;
	down = Tile[mp_Map->at( getXMidPos()>>CSF, getYDownPos()>>CSF, 1)].behaviour;

	// from top
	if(up == 11)
	{
		bdown = true;
		m_basesprite = SWIMBASEFRAME;
	}
	else if(down == 11)
		m_basesprite = WALKBASEFRAME;

	// from right
	if(right == 12)
	{
		bleft = true;
		m_basesprite = SWIMBASEFRAME;
	}
	else if(left == 12)
		m_basesprite = WALKBASEFRAME;

	// from bottom
	if(down == 13)
	{
		bup = true;
		m_basesprite = SWIMBASEFRAME;
	}
	else if(up == 13)
		m_basesprite = WALKBASEFRAME;

	// from left
	if(left == 14)
	{
		bright = true;
		m_basesprite = SWIMBASEFRAME;
	}
	else if(right == 14)
		m_basesprite = WALKBASEFRAME;

	if(m_Inventory.Item.m_special.ep4.swimsuit)
		bleft = bright = bup = bdown = false;
}

/**
 * This performs the animation when player is walking on the map
 */
void CPlayerWM::performWalkingAnimation(bool walking)
{
	if(m_hDir == RIGHT && m_vDir == NONE)
		sprite = m_basesprite + 3;
	else if(m_hDir == NONE && m_vDir == UP)
		sprite = m_basesprite + 6;
	else if(m_hDir == NONE && m_vDir == DOWN)
		sprite = m_basesprite + 9;
	else if(m_hDir == RIGHT && m_vDir == DOWN)
		sprite = m_basesprite + 12;
	else if(m_hDir == LEFT && m_vDir == DOWN)
		sprite = m_basesprite + 15;
	else if(m_hDir == LEFT && m_vDir == UP)
		sprite = m_basesprite + 18;
	else if(m_hDir == RIGHT && m_vDir == UP)
		sprite = m_basesprite + 21;
	else
		sprite = m_basesprite;

	if(walking)
	{
		m_animation_time = 5;
		sprite +=  m_animation%3;
		makeWalkSound();
	}
	else
		sprite +=  2;
}

/**
 * This performs the animation when player is swimming in water on the map
 */
void CPlayerWM::performSwimmingAnimation()
{
	if(m_hDir == RIGHT && m_vDir == NONE)
		sprite = m_basesprite + 2;
	else if(m_hDir == NONE && m_vDir == DOWN)
		sprite = m_basesprite + 4;
	else if(m_hDir == LEFT && m_vDir == NONE)
		sprite = m_basesprite + 6;
	else if(m_hDir == RIGHT && m_vDir == UP)
		sprite = m_basesprite + 8;
	else if(m_hDir == RIGHT && m_vDir == DOWN)
		sprite = m_basesprite + 10;
	else if(m_hDir == LEFT && m_vDir == DOWN)
		sprite = m_basesprite + 12;
	else if(m_hDir == LEFT && m_vDir == UP)
		sprite = m_basesprite + 14;
	else
		sprite = m_basesprite;

	m_animation_time = 5;
	sprite +=  m_animation%2;

	makeSwimSound();
}

}
