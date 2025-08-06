#include <base/audio/Audio.h>
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

CScrub::CScrub(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y) :
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
	mIsDead = false;
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
            playSound(GameSound::SHOT_HIT);
		}
	}

	CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();

	switch(state)
	{
	case SCRUB_DYING:
		mSpriteIdx = SCRUB_FRY_FRAME;
		moveYDir(scrubdie_inertia_y);
		if ( scrubdie_inertia_y < Physics.max_fallspeed )
			scrubdie_inertia_y += Physics.fallspeed_increase;

		dietimer = 0;
		if (scrubdie_inertia_y >= 0 && blockedd)
		{
			mSpriteIdx = SCRUB_DEAD_FRAME;
			state = SCRUB_DEAD;
			mIsDead = true;
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

void CScrub::getTouchedBy(CSpriteObject& theObject)
{
    CCarrier::getTouchedBy(theObject);
}


/*
 * Makes scrub walk to the left
 */
void CScrub::walkLeft(int mx, int my)
{
	mSpriteIdx = SCRUB_WALK_LEFT + walkframe;

	if (blockedl)
	{
		mSpriteIdx = SCRUB_WALK_UP + walkframe;
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
			std::vector<CTileProperties> &TileProperties = gBehaviorEngine.getTileProperties();
			if(!TileProperties[mpMap->at(mx-1, my+1)].bup &&
				!TileProperties[mpMap->at(mx-1, my)].bleft)
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
	mSpriteIdx = SCRUB_WALK_DOWN + walkframe;

	if (blockedd)
	{
		yDirection = 0;
		xDirection = -1;
		mSpriteIdx = SCRUB_WALK_LEFT + walkframe;
	}
	else
	{
		moveCarrierDown(SCRUB_WALK_SPEED);
        processMove((2<<STC),0);

        if(!blockedr) // upper-right, he might be able to stick onto ceiling
		{	// Move right
			yDirection = 0;
			xDirection = 1;
			mSpriteIdx = SCRUB_WALK_RIGHT + walkframe;

            processMove(0,(16<<STC));
            performCollisions();

            // Still unblocked, than put him to the ceiling
            if(!blockedr)
            {
                processMove( (8<<STC), 0 );

                    processMove( 0,-(16<<STC) );
                    performCollisions();
            }

		}

	}
}

/*
 * Makes scrub walk to the right
 */
void CScrub::walkRight(int mx, int my)
{
	mSpriteIdx = SCRUB_WALK_RIGHT + walkframe;

	if (blockedr)
	{
		xDirection = 0;
		yDirection = 1;
		mSpriteIdx = SCRUB_WALK_DOWN + walkframe;
	}
	else
	{
		moveRight(SCRUB_WALK_SPEED);
		processMove(0,-(2<<STC));

		if(!blockedu)
		{
			// First check, if he can walk over the tile
			std::vector<CTileProperties> &TileProperties = gBehaviorEngine.getTileProperties();
			if(!TileProperties[mpMap->at(mx+1, my-1)].bdown &&
               !TileProperties[mpMap->at(mx+1, my)].bright)
			{
                // There is no gap to walk to the other side
                // Try to stick the scrub at the right side of the tile
                processMove(8<<STC,0);
                processMove(0,-(16<<STC));
                processMove(-(8<<STC),0);
                performCollisions();

				if(blockedl)
				{
					xDirection = 0;
					yDirection = -1;
				}
				else
                {
					preparetoFall();
                }
			}
		}
	}
}

/*
 * Makes scrub walk to the up
 */
void CScrub::walkUp()
{
	mSpriteIdx = SCRUB_WALK_UP + walkframe;
	if (blockedu)
	{
		yDirection = 0;
		xDirection = 1;
		mSpriteIdx = SCRUB_WALK_RIGHT + walkframe;
	}
	else
	{
		moveCarrierUp(SCRUB_WALK_SPEED);
        processMove(-(2<<STC), 0);

		if( !blockedl )
		{	// Move Left!
			yDirection = 0;
			xDirection = LEFT;
			mSpriteIdx = SCRUB_WALK_LEFT + walkframe;
			performCollisions();
			processMove(0,-(1<<STC));
			processMove(-(4<<STC),0);
            processMove(0,(4<<STC));
		}

	}
}

/*
 * Makes scrub fall
 */
void CScrub::fall()
{
	mSpriteIdx = SCRUB_WALK_LEFT;
	if (blockedd)
	{
		inhibitfall = true;
		canbezapped = true;

		performCollisions();
		mIsDead = false;
		fallspeed = 0;

		yDirection = 0;
		xDirection = -1;
		state = SCRUB_WALK;
		walkframe = 0;
		animtimer = 0;
	}
	else
	{
		CPhysicsSettings &Physics = gBehaviorEngine.getPhysicsSettings();

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


