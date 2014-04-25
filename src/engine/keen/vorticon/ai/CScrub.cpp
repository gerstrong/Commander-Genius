#include "sdl/audio/Audio.h"
#include "CScrub.h"

// The red creatures that follow the wall (ep2)

enum scrub_actions{
	SCRUB_WALK,          // walking
	SCRUB_FALLING,       // oops, we fell off!
	SCRUB_DYING,         // getting fried!
	SCRUB_DEAD           // dead scrub! here's a dead scrub!
};

#define SCRUB_WALK_ANIM_TIME  11
#define SCRUB_WALK_SPEED      25

#define SCRUBDIE_START_INERTIA      -10
#define SCRUBDIE_INERTIA_DECREASE    2

#define SCRUBPUSHAMOUNT       28

// frames
#define SCRUB_WALK_LEFT       102
#define SCRUB_WALK_UP         104
#define SCRUB_WALK_RIGHT      106
#define SCRUB_WALK_DOWN       108
#define SCRUB_FRY_FRAME       110
#define SCRUB_DEAD_FRAME      111

CScrub::CScrub(CMap *p_map, Uint32 x, Uint32 y) :
CCarrier(p_map, x, y, OBJ_SCRUB),
scrubdie_inertia_y(0)
{
	xDirection = -1;
	state = SCRUB_FALLING;
	walkframe = 0;
	animtimer = 0;
	inhibitfall = true;
	canbezapped = true;

	performCollisions();
	dead = false;
	fallspeed = 0;
}

void CScrub::process()
{    
	CCarrier::process();
    
	if (canbezapped)
	{
		// die if shot
		if (mHealthPoints <= 0 && state!=SCRUB_DYING )
		{
			solid=true;
			state = SCRUB_DYING;
			dietimer = 0;
			moveUp(10);
			scrubdie_inertia_y = SCRUBDIE_START_INERTIA;
			playSound(SOUND_SHOT_HIT);
		}
	}

	CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();

	switch(state)
	{
	case SCRUB_DYING:
		sprite = SCRUB_FRY_FRAME;
		moveYDir(scrubdie_inertia_y);
		if ( scrubdie_inertia_y < Physics.max_fallspeed )
			scrubdie_inertia_y += Physics.fallspeed_increase;

		dietimer = 0;
		if (scrubdie_inertia_y >= 0 && blockedd)
		{
			sprite = SCRUB_DEAD_FRAME;
			state = SCRUB_DEAD;
			dead = true;
		}
		return;
		break;
	case SCRUB_WALK:


		if(xDirection < 0)
			walkLeft( (getXLeftPos())>>CSF, (getYMidPos())>>CSF);
		else if(xDirection > 0)
			walkRight( (getXRightPos())>>CSF, (getYMidPos())>>CSF);
		else if(yDirection < 0)
			walkUp();
		else if(yDirection > 0)
			walkDown();

		// walk animation
		if (animtimer > SCRUB_WALK_ANIM_TIME)
		{
			walkframe ^= 1;
			animtimer = 0;
		} else animtimer++;
		break;

	case SCRUB_FALLING:
		fall();
		break;
	}
}

void CScrub::getTouchedBy(CVorticonSpriteObject& theObject)
{
    CCarrier::getTouchedBy(theObject);
}


/*
 * Makes scrub walk to the left
 */
void CScrub::walkLeft(int mx, int my)
{
	sprite = SCRUB_WALK_LEFT + walkframe;

	if (blockedl)
	{
		sprite = SCRUB_WALK_UP + walkframe;
		xDirection = 0;
		yDirection = -1;
	}
	else
	{
		moveCarrierLeft(SCRUB_WALK_SPEED);		
		processMove(0,(2<<STC));
		performCollisions();

		if(!blockedd)
		{
			// First check, if he can walk over the tile
			std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
			if(!TileProperties[mp_Map->at(mx-1, my+1)].bup &&
				!TileProperties[mp_Map->at(mx-1, my)].bleft)
			{
				// There is no gap
				processMove(0,4<<STC);
				processMove(4<<STC,0);
				performCollisions();
				if(blockedr)
				{
					xDirection = 0;
					yDirection = 1;
				}
				else
					preparetoFall();
			}
		}
	}
}

/*
 * Makes scrub walk to the down
 */
void CScrub::walkDown()
{
	sprite = SCRUB_WALK_DOWN + walkframe;

	if (blockedd)
	{
		yDirection = 0;
		xDirection = -1;
		sprite = SCRUB_WALK_LEFT + walkframe;
	}
	else
	{
		moveCarrierDown(SCRUB_WALK_SPEED);

		if(!blockedr) // upper-right, if yes, go right! (ceiling)
		{	// Move right
			yDirection = 0;
			xDirection = 1;
			sprite = SCRUB_WALK_RIGHT + walkframe;
			processMove(2<<STC,0);
			processMove(0,-(2<<STC));
		}

	}
}

/*
 * Makes scrub walk to the right
 */
void CScrub::walkRight(int mx, int my)
{
	sprite = SCRUB_WALK_RIGHT + walkframe;

	if (blockedr)
	{
		xDirection = 0;
		yDirection = 1;
		sprite = SCRUB_WALK_DOWN + walkframe;
	}
	else
	{
		moveRight(SCRUB_WALK_SPEED);
		processMove(0,-(2<<STC));

		if(!blockedu)
		{
			// First check, if he can walk over the tile
			std::vector<CTileProperties> &TileProperties = g_pBehaviorEngine->getTileProperties();
			if(!TileProperties[mp_Map->at(mx+1, my-1)].bdown &&
				!TileProperties[mp_Map->at(mx+1, my)].bright)
			{
				// There is no gap the upper-side
				processMove(4<<STC,0);
				processMove(0,-(4<<STC));
				processMove(-(6<<STC),0);
				performCollisions();

				if(blockedl)
				{
					xDirection = 0;
					yDirection = -1;
				}
				else
					preparetoFall();
			}
		}
	}
}

/*
 * Makes scrub walk to the up
 */
void CScrub::walkUp()
{
	sprite = SCRUB_WALK_UP + walkframe;
	if (blockedu)
	{
		yDirection = 0;
		xDirection = 1;
		sprite = SCRUB_WALK_RIGHT + walkframe;
	}
	else
	{
		moveCarrierUp(SCRUB_WALK_SPEED);

		if( !blockedl )
		{	// Move Left!
			yDirection = 0;
			xDirection = LEFT;
			sprite = SCRUB_WALK_LEFT + walkframe;
			performCollisions();
			processMove(0,-(1<<STC));
			processMove(-(4<<STC),0);
			processMove(0,4<<STC);
		}

	}
}

/*
 * Makes scrub fall
 */
void CScrub::fall()
{
	sprite = SCRUB_WALK_LEFT;
	if (blockedd)
	{
		inhibitfall = true;
		canbezapped = true;

		performCollisions();
		dead = false;
		fallspeed = 0;

		yDirection = 0;
		xDirection = -1;
		state = SCRUB_WALK;
		walkframe = 0;
		animtimer = 0;
	}
	else
	{
		CPhysicsSettings &Physics = g_pBehaviorEngine->getPhysicsSettings();

		if (fallspeed < Physics.max_fallspeed)
			fallspeed += Physics.fallspeed_increase;

		moveDown(fallspeed);
	}
}
/**
 * Prepares the grub to fall!
 */
void CScrub::preparetoFall()
{
	fallspeed = 0;
	state = SCRUB_FALLING;
	yDirection = 0;
	xDirection = -1;
}


