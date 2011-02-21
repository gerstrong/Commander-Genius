/*
 * CPlayer.cpp
 *
 *  Created on: 14.07.2010
 *      Author: gerstrong
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

CPlayerWM::CPlayerWM(CMap *pmap, Uint32 x, Uint32 y,
					std::vector<CObject*>& ObjectPtrs,
					CInventory &l_Inventory):
CObject(pmap, x, y, OBJ_PLAYER),
m_Inventory(l_Inventory),
m_basesprite(WALKBASEFRAME),
m_looking_dir(LEFT),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_ObjectPtrs(ObjectPtrs),
m_camera(pmap,x,y,this)
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

	processWalking();

	// Events for the Player are processed here.
	CEventContainer& EventContainer = g_pBehaviorEngine->m_EventList;
	if( EventExitLevel* ev = EventContainer.occurredEvent<EventExitLevel>() )
	{
		finishLevel(ev->levelObject);
		EventContainer.pop_Event();
	}

	m_camera.process();
}

/*
 * Processes the walking of the player on map here
 */
void CPlayerWM::processWalking()
{
	size_t movespeed = m_basesprite == WALKBASEFRAME ? 50 : 25;
	bool walking=false;

	// Normal walking
	if(g_pInput->getHoldedCommand(IC_LEFT))
	{
		moveLeft(movespeed);
		walking = true;
		m_hDir = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT))
	{
		moveRight(movespeed);
		walking = true;
		m_hDir = RIGHT;
	}
	else
		m_hDir = NONE;

	if(g_pInput->getHoldedCommand(IC_UP))
	{
		if(!walking)
			m_hDir = NONE;

		moveUp(movespeed);
		walking = true;
		m_vDir = UP;
	}
	else if(g_pInput->getHoldedCommand(IC_DOWN))
	{
		if(!walking)
			m_hDir = NONE;

		moveDown(movespeed);
		walking = true;
		m_vDir = DOWN;
	}
	else
		m_vDir = NONE;


	if(g_pInput->getHoldedCommand(IC_STATUS))
		solid = !solid;

	// perform actions depending if the action button was pressed
	if(g_pInput->getPressedCommand(IC_JUMP))
	{
		Uint16 object = mp_Map->getPlaneDataAt(2, getXMidPos(), getYMidPos());
		if(object)
		{
			startLevel(object);
			g_pSound->playSound( SOUND_ENTER_LEVEL );
			g_pInput->flushCommands();
		}
	}

	// this means if keen is just walking on the map or swimming in the sea
	if(m_basesprite == WALKBASEFRAME)
		performWalkingAnimation(walking);
	else if(m_basesprite == SWIMBASEFRAME)
		performSwimmingAnimation();

	// This will trigger between swim and walkmode
	checkforSwimming();
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

		// Mark the tileinfo on the map as marked!!
		mp_Map->setTile( x, y, 0, true, 2);
	}
}
/**
 * This is the function will switch between swim and walk mode
 * Those are the tileproperties to check for
 * 11      Enter water from top	Keen 4
 * 12      Enter water from right	Keen 4
 * 13      Enter water from bottom Keen 4
 * 14      Enter water from left	Keen 4
 */
void CPlayerWM::checkforSwimming()
{
	Uint16 left, right, up, down;
	std::vector<CTileProperties> &Tile = g_pBehaviorEngine->getTileProperties(1);

	left = Tile[mp_Map->at( getXLeftPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
	right = Tile[mp_Map->at( getXRightPos()>>CSF, getYMidPos()>>CSF, 1)].behaviour;
	up = Tile[mp_Map->at( getXMidPos()>>CSF, getYUpPos()>>CSF, 1)].behaviour;
	down = Tile[mp_Map->at( getXMidPos()>>CSF, getYDownPos()>>CSF, 1)].behaviour;

	// from top
	if(up == 11)
		m_basesprite = SWIMBASEFRAME;
	else if(down == 11)
		m_basesprite = WALKBASEFRAME;

	// from right
	if(right == 12)
		m_basesprite = SWIMBASEFRAME;
	else if(left == 12)
		m_basesprite = WALKBASEFRAME;

	// from bottom
	if(down == 13)
		m_basesprite = SWIMBASEFRAME;
	else if(up == 13)
		m_basesprite = WALKBASEFRAME;

	// from left
	if(left == 14)
		m_basesprite = SWIMBASEFRAME;
	else if(right == 14)
		m_basesprite = WALKBASEFRAME;
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
	}
	else
		sprite +=  2;
}

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
}

CPlayerWM::~CPlayerWM() {
	// TODO Auto-generated destructor stub
}

}
