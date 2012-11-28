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
		CInventory &l_Inventory,
		stCheat &Cheatmode,
		const unsigned int actionoffset):
CPlayerBase(pmap, foeID, x, y,
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

	walkBaseFrame = sprite;
	swimBaseFrame = walkBaseFrame + 24;
	m_basesprite = walkBaseFrame;

	performCollisions();
	mProcessPtr = &CPlayerWM::processMoving;
}


/**
 * Before Keen rides on the foot we get the location where to ride
 */
VectorD2<int> CPlayerWM::fetchFootDestCoord()
{
	VectorD2<int> location1;
	VectorD2<int> location2;
	int coordData;

	const byte *dataPtr = g_pBehaviorEngine->m_ExeFile.getRawData();
	//const byte *dataPtr = g_pBehaviorEngine->m_ExeFile.getDSegPtr(); // only Zeros here!
	//const byte *dataPtr = (byte*) g_pBehaviorEngine->m_ExeFile.getHeaderData();

	memcpy(&coordData, dataPtr+0xDE43, sizeof(int));
	location1.x = coordData & 0xFF;
	memcpy(&coordData, dataPtr+0xDE58, sizeof(int));
	location1.y = coordData & 0xFF;

	memcpy(&coordData, dataPtr+0xDE78, sizeof(int));
	location2.x = coordData & 0xFF;
	memcpy(&coordData, dataPtr+0xDE8C, sizeof(int));
	location2.y = coordData & 0xFF;


	// Check for the first location
	VectorD2<int> vec1;
	VectorD2<int> vec2;

	VectorD2<int> levelCoord;

	levelCoord = getPosition();

	levelCoord.x >>= CSF;
	levelCoord.y >>= CSF;

	vec1.x = levelCoord.x - location1.x;
	vec1.y = levelCoord.y - location1.y;
	vec2.x = levelCoord.x - location2.x;
	vec2.y = levelCoord.y - location2.y;

	const int dist1 = vec1.GetLength2();
	const int dist2 = vec2.GetLength2();

	VectorD2<int> newCoord;

	if(dist2 > dist1)
		newCoord = location2;
	else
		newCoord = location1;

	newCoord.x <<= CSF;
	newCoord.y <<= CSF;
	return newCoord;
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

	(this->*mProcessPtr)();

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

	if( EventPlayerRideFoot* ev = EventContainer.occurredEvent<EventPlayerRideFoot>() )
	{
		finishLevel(ev->levelObject);
		solid = false;

		// Here we need to set the coordinates calculated to where Keen has to go.
		target = fetchFootDestCoord();

		//setAction(A_KEEN_RIDING_ON_FOOT);
		// 0x1492 seems to be the foot
		m_Action.setActionFormat(0x1492);
		setActionSprite();
		mProcessPtr = &CPlayerWM::processRiding;
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

	// In Episode 5 and 5 there are teleporters. Verify those teleporters and elevators
	if(g_pBehaviorEngine->getEpisode() >= 5)
		verifyTeleportation();

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
				EventContainer.add( new EventSendBitmapDialogMsg(g_pGfxEngine->getBitmap(105),
						g_pBehaviorEngine->getString("CANT_SWIM_TEXT"), LEFT) );

				m_cantswim = true;
			}

		}
	}
}

const int RIDE_SPEED = 32;

void CPlayerWM::processRiding()
{
	// Ride while trying to reach the destination coords
	// Move the player to the target
	VectorD2<int> pos(getXPosition(), getYPosition());
	VectorD2<int> vec = target-pos;

	VectorD2<int> vec_norm = vec;

	const int dist_x = abs(vec.x);
	const int dist_y = abs(vec.y);

	if(dist_x != 0)
		vec_norm.x = vec.x/dist_x;
	if(dist_y != 0)
		vec_norm.y = vec.y/dist_y;

	if( dist_x < RIDE_SPEED &&	dist_y < RIDE_SPEED)
	{
		// When he reaches the target. make him visible and start opening the elevator
		moveDir(vec);
		setAction(0);
		mProcessPtr = &CPlayerWM::processMoving;
		solid = true;
	}
	else
	{
		moveDir(vec_norm*RIDE_SPEED);
		processActionRoutine();
	}
}


void CPlayerWM::verifyTeleportation()
{
	//target -> change it when the touched tile is known
	int x = getXMidPos();
	int y = getYUpPos();

	// Check if Keen touches a teleporter or elevator
	const Uint16 object = mp_Map->getPlaneDataAt( 2, x, y );
	if(object < 0xC000 && object > 0x100)
	{
		x = (x >> CSF);
		y = (y >> CSF);

		bool isElevator = false;

		// Elevator are double the size. Check that! Else it must be an teleporter
		if(object == mp_Map->getPlaneDataAt( 2, (x-1) << CSF, y << CSF ))
		{
			isElevator |= true;
		}
		if(object == mp_Map->getPlaneDataAt( 2, (x+1) << CSF, y << CSF ))
		{
			x = x + 1;
			isElevator |= true;
		}

		x = (x << CSF);
		y = (y << CSF);

		target.x = x;
		target.y = y;

		// In that case get the tile where to go and make him move or ...
		if(isElevator)
		{
			mProcessPtr = &CPlayerWM::processEnteringElevator;
		}
		else // ... make him move until teleporter hides him.
		{
			mProcessPtr = &CPlayerWM::processEnteringTeleporter;
			// TODO: Activate teleporter animation
		}
	}

}


// Elevators
const int SLOW_TELEPORT_WALK_SPEED = 8;
const int ELEVATOR_SPEED = 128;
const int ELEVATOR_CLOSE_TIME = 5;

void CPlayerWM::processEnteringElevator()
{
	// Move him to the target
	VectorD2<int> pos(getXPosition(), getYPosition());
	VectorD2<int> vec = target-pos;


	VectorD2<int> vec_norm = vec;

	const int dist_x = abs(vec.x);
	const int dist_y = abs(vec.y);

	if(dist_x != 0)
		vec_norm.x = vec.x/dist_x;
	if(dist_y != 0)
		vec_norm.y = vec.y/dist_y;

	yDirection = vec_norm.y;


	if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
		dist_y < SLOW_TELEPORT_WALK_SPEED)
	{
		moveDir(vec);
		// TODO: Play Elevator open sound
		mProcessPtr = &CPlayerWM::processClosingElevator;
		performWalkingAnimation(false);
		elevator_frames = 5;
		elevator_close_timer = 0;
	}
	else
	{
		moveDir(vec_norm*SLOW_TELEPORT_WALK_SPEED);
	}

	performWalkingAnimation(true);
}

void CPlayerWM::processClosingElevator()
{
	const int x = getXMidPos() >> CSF;
	const int y = getYMidPos() >> CSF;
	const Uint16 tile1 = mp_Map->getPlaneDataAt( 1, x<<CSF, y<<CSF );
	const Uint16 tile2 = mp_Map->getPlaneDataAt( 1, (x-1)<<CSF, y<<CSF );
	const Uint16 tile3 = mp_Map->getPlaneDataAt( 1, (x-1)<<CSF, (y-1)<<CSF );
	const Uint16 tile4 = mp_Map->getPlaneDataAt( 1, x<<CSF, (y-1)<<CSF );

	elevator_close_timer++;
	if(elevator_close_timer >= ELEVATOR_CLOSE_TIME)
	{
		elevator_close_timer = 0;

		// Make the player close the elevator
		mp_Map->setTile(x, y, tile1-2, true);
		mp_Map->setTile(x-1, y, tile2-2, true);
		mp_Map->setTile(x-1, y-1, tile3-2, true);
		mp_Map->setTile(x, y-1, tile4-2, true);

		elevator_frames--;

		if(elevator_frames == 0)
		{
			// If done make him invisible and transport him through the level. !solid

			// TODO: get new target where to move
			//target -> change it when the touched tile is known
			const int x = getXMidPos();
			const int y = getYMidPos();

			// Check if Keen touches a teleporter or elevator
			const Uint16 object = mp_Map->getPlaneDataAt( 2, x, y );
			const Uint32 filter = object & 0xFFFF;
			const Uint32 newPosX = (filter & 0xFF00) >> 8;
			const Uint32 newPosY = (filter & 0x00FF);

			// Set new target
			target.x = ((newPosX+1)<<CSF);
			target.y = (newPosY<<CSF);

			// make him invisible
			solid = false;
			dontdraw = true;

			// change process
			mProcessPtr = &CPlayerWM::processElevating;
		}
	}
}

void CPlayerWM::processElevating()
{
	// Move the player to the target
	VectorD2<int> pos(getXPosition(), getYPosition());
	VectorD2<int> vec = target-pos;


	VectorD2<int> vec_norm = vec;

	const int dist_x = abs(vec.x);
	const int dist_y = abs(vec.y);

	if(dist_x != 0)
		vec_norm.x = vec.x/dist_x;
	if(dist_y != 0)
		vec_norm.y = vec.y/dist_y;

	if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
		dist_y < SLOW_TELEPORT_WALK_SPEED)
	{
		// TODO: When he reaches the target. make him visible and start opening the elevator
		moveDir(vec);
		// TODO: Play Elevator open sound
		mProcessPtr = &CPlayerWM::processOpeningElevator;
		performWalkingAnimation(false);
		elevator_frames = 5;
		elevator_close_timer = 0;
	}
	else
	{
		moveDir(vec_norm*ELEVATOR_SPEED);
	}
}

void CPlayerWM::processOpeningElevator()
{
	// Open until it's wide open
	const int x = getXMidPos() >> CSF;
	const int y = getYMidPos() >> CSF;
	const Uint16 tile1 = mp_Map->getPlaneDataAt( 1, x<<CSF, y<<CSF );
	const Uint16 tile2 = mp_Map->getPlaneDataAt( 1, (x-1)<<CSF, y<<CSF );
	const Uint16 tile3 = mp_Map->getPlaneDataAt( 1, (x-1)<<CSF, (y-1)<<CSF );
	const Uint16 tile4 = mp_Map->getPlaneDataAt( 1, x<<CSF, (y-1)<<CSF );

	elevator_close_timer++;
	if(elevator_close_timer >= ELEVATOR_CLOSE_TIME)
	{
		elevator_close_timer = 0;

		// Make the player close the elevator
		mp_Map->setTile(x, y, tile1+2, true);
		mp_Map->setTile(x-1, y, tile2+2, true);
		mp_Map->setTile(x-1, y-1, tile3+2, true);
		mp_Map->setTile(x, y-1, tile4+2, true);

		elevator_frames--;

		if(elevator_frames == 0)
		{
			// make him visible
			dontdraw = false;

			target.y += (1<<CSF);

			// and walk out
			mProcessPtr = &CPlayerWM::processLeavingElevator;
		}
	}
}

void CPlayerWM::processLeavingElevator()
{
	// Move him to the target
	VectorD2<int> pos(getXPosition(), getYPosition());
	VectorD2<int> vec = target-pos;


	VectorD2<int> vec_norm = vec;

	const int dist_x = abs(vec.x);
	const int dist_y = abs(vec.y);

	if(dist_x != 0)
		vec_norm.x = vec.x/dist_x;
	if(dist_y != 0)
		vec_norm.y = vec.y/dist_y;

	yDirection = vec_norm.y;

	if( dist_x < SLOW_TELEPORT_WALK_SPEED &&
		dist_y < SLOW_TELEPORT_WALK_SPEED)
	{
		// When done set him solid
		solid = true;
		moveDir(vec);
		mProcessPtr = &CPlayerWM::processMoving;
	}
	else
	{
		moveDir(vec_norm*SLOW_TELEPORT_WALK_SPEED);
	}

	performWalkingAnimation(true);

	// TODO: When done set him solid
}


// Teleporter

void CPlayerWM::processEnteringTeleporter()
{
	// TODO: he moves to target into the teleporter

	// TODO: When finished make him !solid and invisible

	// TODO: Deactivate animation
}


void CPlayerWM::processWarpInTeleporter()
{
	// TODO: This should just change the location

	// TODO: When done make visible again and activate teleport animation
}

void CPlayerWM::processLeavingTeleporter()
{
	// TODO: he moves to target out the teleporter

	// TODO: When finished make him solid
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
		csfY += (2<<STC);
		VectorD2<Uint32> dst(csfX, csfY);


		CFlag *pFlag = new CFlag(mp_Map, src, dst);		
		g_pBehaviorEngine->m_EventList.spawnObj(pFlag);
		g_pSound->playSound( SOUND_FLAG_APPEAR );

		// Mark the tileinfo on the map as marked!!
		mp_Map->setTile( x, y, 0, true, 2);
	}	
}

/**
 * This is the function will switch between swim and walk mode
 * Those are the tileproperties to check for
 * 11      Enter water from top	Keen 4
 * 12      Enter water from right Keen 4
 * 13      Enter water from bottom Keen 4
 * 14      Enter water from left Keen 4
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
		playWalkSound();
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

	playSwimSound();
}

}
