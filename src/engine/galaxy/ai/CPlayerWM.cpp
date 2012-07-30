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
#include "sdl/input/CInput.h"
#include "sdl/sound/CSound.h"
#include "CVec.h"

namespace galaxy {

CPlayerWM::CPlayerWM(CMap *pmap,
		const Uint16 foeID,
		Uint32 x,
		Uint32 y,
		std::vector< SmartPointer<CGalaxySpriteObject> > &ObjectPtrs,
		CInventory &l_Inventory,
		stCheat &Cheatmode,
		const unsigned int actionoffset):
CPlayerBase(pmap, foeID, x, y,
		    ObjectPtrs,
		    LEFT,
		    l_Inventory,
		    Cheatmode),
m_basesprite(0),
walkBaseFrame(0),
m_looking_dir(LEFT),
m_animation(0),
m_animation_time(1),
m_animation_ticker(0),
m_cantswim(false)
{
	m_ActionBaseOffset = actionoffset;
	CGalaxySpriteObject::setActionForce(0);
	setActionSprite();

	// TODO: Temporary workaround
	walkBaseFrame = sprite;
	swimBaseFrame = walkBaseFrame + 24;
	m_basesprite = walkBaseFrame;
	CSprite &rSprite = g_pGfxEngine->getSprite(sprite);
	performCollisions();
	processMove( 0, rSprite.m_bboxY1-rSprite.m_bboxY2 );
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
		    const std::string loading_text = g_pBehaviorEngine->getString("WORLDMAP_LOAD_TEXT");
			EventContainer.add( new EventSendBitmapDialogMsg(106, loading_text, LEFT) );
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
	if(m_basesprite == swimBaseFrame)
		movespeed = 25;
	else if(m_basesprite == walkBaseFrame)
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
			yDirection = 0;

		moveLeft(movespeed);
		walking = true;
		xDirection = LEFT;
	}
	else if(g_pInput->getHoldedCommand(IC_RIGHT) && !bright)
	{
		if(!g_pInput->getHoldedCommand(IC_UP) && !g_pInput->getHoldedCommand(IC_DOWN))
			yDirection = 0;

		moveRight(movespeed);
		walking = true;
		xDirection = RIGHT;
	}

	if(g_pInput->getHoldedCommand(IC_UP) && !bup)
	{
		if(!g_pInput->getHoldedCommand(IC_LEFT) && !g_pInput->getHoldedCommand(IC_RIGHT))
			xDirection = 0;

		moveUp(movespeed);
		walking = true;
		yDirection = UP;
	}
	else if(g_pInput->getHoldedCommand(IC_DOWN) && !bdown)
	{
		if(!g_pInput->getHoldedCommand(IC_LEFT) && !g_pInput->getHoldedCommand(IC_RIGHT))
			xDirection = 0;

		moveDown(movespeed);
		walking = true;
		yDirection = DOWN;
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
	if(m_basesprite == walkBaseFrame)
	{
		performWalkingAnimation(walking);
		m_cantswim = false;
	}
	else if(m_basesprite == swimBaseFrame)
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
				EventContainer.add( new EventSendBitmapDialogMsg(105,
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

		// Here we move the coordinates in order get it positioned correctly in the pole
		CSprite &FlagSprite = g_pGfxEngine->getSprite(WAVING_BASEFRAME);

		unsigned int csfX = (x<<CSF);
		unsigned int csfY = (y<<CSF);

		csfX += (6<<STC);
		csfY -= FlagSprite.m_bboxY2;
		csfY += (1<<STC);
		VectorD2<Uint32> dst(csfX, csfY);


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
		m_basesprite = swimBaseFrame;
	}
	else if(down == 11)
		m_basesprite = walkBaseFrame;

	// from right
	if(right == 12)
	{
		bleft = true;
		m_basesprite = swimBaseFrame;
	}
	else if(left == 12)
		m_basesprite = walkBaseFrame;

	// from bottom
	if(down == 13)
	{
		bup = true;
		m_basesprite = swimBaseFrame;
	}
	else if(up == 13)
		m_basesprite = walkBaseFrame;

	// from left
	if(left == 14)
	{
		bright = true;
		m_basesprite = swimBaseFrame;
	}
	else if(right == 14)
		m_basesprite = walkBaseFrame;

	if(m_Inventory.Item.m_special.ep4.swimsuit)
		bleft = bright = bup = bdown = false;
}

/**
 * This performs the animation when player is walking on the map
 */
void CPlayerWM::performWalkingAnimation(bool walking)
{
	if(xDirection == RIGHT && yDirection == 0)
		sprite = m_basesprite + 1;
	else if(xDirection == 0 && yDirection == UP)
		sprite = m_basesprite + 4;
	else if(xDirection == 0 && yDirection == DOWN)
		sprite = m_basesprite + 7;
	else if(xDirection == RIGHT && yDirection == DOWN)
		sprite = m_basesprite + 10;
	else if(xDirection == LEFT && yDirection == DOWN)
		sprite = m_basesprite + 13;
	else if(xDirection == LEFT && yDirection == UP)
		sprite = m_basesprite + 16;
	else if(xDirection == RIGHT && yDirection == UP)
		sprite = m_basesprite + 19;
	else
		sprite = m_basesprite - 2;

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
	if(xDirection == RIGHT && yDirection == 0)
		sprite = m_basesprite + 2;
	else if(xDirection == 0 && yDirection == DOWN)
		sprite = m_basesprite + 4;
	else if(xDirection == LEFT && yDirection == 0)
		sprite = m_basesprite + 6;
	else if(xDirection == RIGHT && yDirection == UP)
		sprite = m_basesprite + 8;
	else if(xDirection == RIGHT && yDirection == DOWN)
		sprite = m_basesprite + 10;
	else if(xDirection == LEFT && yDirection == DOWN)
		sprite = m_basesprite + 12;
	else if(xDirection == LEFT && yDirection == UP)
		sprite = m_basesprite + 14;
	else
		sprite = m_basesprite;

	m_animation_time = 5;
	sprite +=  m_animation%2;

	makeSwimSound();
}

}
