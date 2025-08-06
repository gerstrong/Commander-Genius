#include <base/interface/misc.h>

#include <base/audio/Audio.h>
#include <base/video/CVideoDriver.h>
#include "engine/core/spritedefines.h"
//#include "graphics/GsGraphics.h"
//#include "CPlayer.h"

#include "CVorticonElite.h"
#include "CRay.h"

#define VORTELITE_JUMP_PROB          15
#define VORTELITE_FIRE_PROB          8

#define VORTELITE_MIN_TIME_BETWEEN_FIRE    	25
#define VORTELITE_HOLD_GUN_OUT_TIME         22
#define VORTELITE_HOLD_GUN_AFTER_FIRE_TIME  20

#define VORTELITE_MIN_JUMP_HEIGHT    90
#define VORTELITE_MAX_JUMP_HEIGHT    150
#define VORTELITE_MAX_FALL_SPEED     240
#define VORTELITE_JUMP_FRICTION      2

const int WALK_SPEED = 31;
const int CHARGE_SPEED = 62;
const unsigned int VORTELITE_WALK_ANIM_TIME = 6;

// number of shots to kill
#define VORTELITE_HP                 3

#define VORTELITE_DIE_ANIM_TIME     45

#define VORTELITE_LOOK_ANIM_TIME     60

#define VORTELITE_WALK_LEFT_FRAME    88
#define VORTELITE_WALK_RIGHT_FRAME   92
#define VORTELITE_JUMP_RIGHT_FRAME   98
#define VORTELITE_JUMP_LEFT_FRAME    99
#define VORTELITE_DYING_FRAME        100
#define VORTELITE_DEAD_FRAME         101
#define VORTELITE_FIRE_LEFT_FRAME    96
#define VORTELITE_FIRE_RIGHT_FRAME   97

#define VORTELITE_PALETTE_FLASH_TIME  5

#define VORTELITE_TRAPPED_DIST        150

const int PLAYER_DISTANCE = (6<<CSF); // distance the player should stay away, so Vortelite won't run.

CVorticonElite::CVorticonElite( std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y ) :
CVorticon(p_map, x, y, 4, OBJ_VORTELITE)
{
	state = VORTELITE_WALK;
	movedir = LEFT;
	mSpriteIdx = VORTELITE_WALK_LEFT_FRAME;
	frame = 0;
	animtimer = 0;
	timer = 0;
	timesincefire = 0;
	dist_traveled = VORTELITE_TRAPPED_DIST+1;
	canbezapped = true;
	m_speed = 0;

    const auto diff = gBehaviorEngine.mDifficulty;


    switch(diff)
    {
    case EASY:
        mHealthPoints = 1;
        break;
    case NORMAL:
        break;
    case HARD:
        mHealthPoints++;
        break;
    case EXPERT:
        mSprVar = 1;
        mHealthPoints += 2;
        break;
    case NINJA:
        mSprVar = 2;
        mHealthPoints += 3;
        break;
    case ELITE:
        mSprVar = 3;
        mHealthPoints += 4;
        break;
    default: break;
    }
}


bool CVorticonElite::isNearby(CSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
        if(state == VORTELITE_WALK)
        {

            if(getYDownPos() > player->getYDownPos()-(2<<CSF) and
                    getYDownPos() < player->getYDownPos()+(2<<CSF) )
            {
                int dist;
                if(getXMidPos() > player->getXMidPos())
                {
                    if (rand()%10 != 0)
                        movedir = LEFT;

                    dist = getXMidPos()-player->getXMidPos();
                }
                else
                {
                    if (rand()%10 != 0)
                        movedir = RIGHT;

                    dist = player->getXMidPos()-getXMidPos();
                }

                // If Player is nearby, make vorticon go faster
                if(dist < PLAYER_DISTANCE)
                    state = VORTELITE_CHARGE;
            }

            dist_traveled++;

            const auto diff = gBehaviorEngine.mDifficulty;

            if(!mpMap->m_Dark || diff > NORMAL)
            {
                if (getProbability(VORTELITE_JUMP_PROB) && !blockedu)
                {  // let's jump.
                    initiatejump();
                    return true;
                }
                else
                {
                    if (timesincefire > VORTELITE_MIN_TIME_BETWEEN_FIRE)
                    {
                        if (getProbability(VORTELITE_FIRE_PROB))
                        {  	// let's fire
                            // usually shoot toward keen
                            if (rand()%5 != 0)
                            {
                                if (getXPosition() < player->getXPosition())
                                    movedir = RIGHT;
                                else
                                    movedir = LEFT;
                            }
                            timer = 0;
                            state = VORTELITE_ABOUTTOFIRE;
                        }
                    }
                    else
                    {
                        timesincefire++;
                    }
                }
            }
        }
    }
    
    return true;
}

void CVorticonElite::process()
{
	if (mHealthPoints <= 0 && state != VORTELITE_DYING)
	{
		animtimer = 0;
		frame = 0;
		state = VORTELITE_DYING;
		dying = true;

		if (onscreen)
            playSound(GameSound::VORT_DIE);
	}

	if(state == VORTELITE_CHARGE)
	{
		m_speed = CHARGE_SPEED;
	}
	else if(state == VORTELITE_WALK)
	{
		m_speed = WALK_SPEED;
	}

	reprocess: ;
	switch(state)
	{
	case VORTELITE_CHARGE:
	case VORTELITE_WALK:

		if (movedir==LEFT)
		{  // move left
			mSpriteIdx = VORTELITE_WALK_LEFT_FRAME + frame;
			if (!blockedl)
			{
				xinertia = -m_speed;
			}
			else
			{
				movedir = RIGHT;

				// if we only traveled a tiny amount before hitting a wall, we've
				// probably fallen into a small narrow area, and we need to try
				// to jump out of it
				if (dist_traveled < VORTELITE_TRAPPED_DIST && !mpMap->m_Dark && blockedd)
				{
					initiatejump();
					goto reprocess;
				}
                else if(mpMap->m_Dark)
                {
                    dist_traveled = 0;
                }
			}
		}
		else
		{  // move right
			mSpriteIdx = VORTELITE_WALK_RIGHT_FRAME + frame;
			if (!blockedr)
			{
				xinertia = m_speed;
			}
			else
			{
				movedir = LEFT;

				// if we only traveled a tiny amount before hitting a wall, we've
				// probably fallen into a small narrow area, and we need to try
				// to jump out of it
				if (dist_traveled < VORTELITE_TRAPPED_DIST && !mpMap->m_Dark && blockedd)
				{
					initiatejump();
					goto reprocess;
				}
                else if(mpMap->m_Dark)
                {
                    dist_traveled = 0;
                }
            }
		}

		// walk animation
		if (animtimer > VORTELITE_WALK_ANIM_TIME)
		{
			if (frame>=3) frame=0;
			else frame++;
			animtimer = 0;
		} else animtimer++;
		break;

	case VORTELITE_JUMP:
		if (movedir == RIGHT)
		{ if (!blockedr) moveRight(m_speed); }
		else
		{ if (!blockedl) moveLeft(m_speed); }

		if (blockedd && yinertia >= 0)
		{  // The Vorticon Has landed after the jump!
			state = VORTELITE_WALK;
			goto reprocess;
		}

		break;
	case VORTELITE_ABOUTTOFIRE:
		if (movedir==RIGHT)
		{ mSpriteIdx = VORTELITE_FIRE_RIGHT_FRAME; }
		else
		{ mSpriteIdx = VORTELITE_FIRE_LEFT_FRAME; }
		if (timer > VORTELITE_HOLD_GUN_OUT_TIME)
		{
			timer = 0;
			state = VORTELITE_FIRED;

			CRay *newobject;
			if (movedir==RIGHT)
                newobject = new CRay(mpMap, getXRightPos()+1, getYPosition()+(9<<STC), RIGHT, CENTER, getSpriteVariantIdx());
			else
                newobject = new CRay(mpMap, getXLeftPos()-1, getYPosition()+(9<<STC), LEFT, CENTER, getSpriteVariantIdx());
			newobject->setOwner( m_type, m_index);
			newobject->mSpriteIdx = ENEMYRAYEP2;
			// don't shoot other vorticon elite
			spawnObj(newobject);

			if (onscreen)
                playSound(GameSound::KEEN_FIRE);
		}
		else timer++;
		break;
	case VORTELITE_FIRED:
		if (movedir==RIGHT)
		{ mSpriteIdx = VORTELITE_FIRE_RIGHT_FRAME; }
		else
		{ mSpriteIdx = VORTELITE_FIRE_LEFT_FRAME; }

		if (timer > VORTELITE_HOLD_GUN_AFTER_FIRE_TIME)
		{
			timer = 0;
			frame = 0;
			timesincefire = 0;
			state = VORTELITE_WALK;
		}
		else timer++;
		break;
	case VORTELITE_DYING:
		mSpriteIdx = VORTELITE_DYING_FRAME;
		if (animtimer > VORTELITE_DIE_ANIM_TIME)
		{
			mSpriteIdx = VORTELITE_DEAD_FRAME;
			mIsDead = true;
		}
		else
		{
			animtimer++;
		}
		break;
	default: break;
	}
}

void CVorticonElite::initiatejump()
{
	if (state==VORTELITE_JUMP) return;

	frame = 0;
	animtimer = 0;
	yinertia = -((rand()%(VORTELITE_MAX_JUMP_HEIGHT-VORTELITE_MIN_JUMP_HEIGHT))+VORTELITE_MIN_JUMP_HEIGHT);

	if (movedir==RIGHT)
		mSpriteIdx = VORTELITE_JUMP_RIGHT_FRAME;
	else
		mSpriteIdx = VORTELITE_JUMP_LEFT_FRAME;

	state = VORTELITE_JUMP;
}

void CVorticonElite::getShotByRay(object_t &obj_type)
{
	if( !mInvincible && mHealthPoints>0 && obj_type != OBJ_GUARDROBOT)
	{
        if(mHealthPoints>1 && gBehaviorEngine.mOptions[GameOption::SPECIALFX].value)
			blink(10);
		mHealthPoints--;
	}
}


