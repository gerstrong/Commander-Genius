#include <base/audio/Audio.h>

#include "CVortiNinja.h"

#define NINJA_STAND_ANIM_RATE          8
#define NINJA_DYING_SHOW_TIME          6
#define NINJA_MIN_TIME_TILL_KICK       80
#define NINJA_MAX_TIME_TILL_KICK       100

#define NINJA_KICK_MOVE_RATE           1

#define NINJA_STAND_LEFT_FRAME         77
#define NINJA_STAND_RIGHT_FRAME        79
#define NINJA_KICK_LEFT_FRAME          81
#define NINJA_KICK_RIGHT_FRAME         82
#define NINJA_DYING_FRAME              83
#define NINJA_DEAD_FRAME               84

unsigned int rnd(void);

CVortiNinja::CVortiNinja(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y) :
CVorticonSpriteObject(p_map, x, y, OBJ_NINJA),
longjump(false)
{
	canbezapped = true;
	mHealthPoints = 4;
	if(gBehaviorEngine.mDifficulty > NORMAL)
		mHealthPoints++;
	else if(gBehaviorEngine.mDifficulty < NORMAL)
		mHealthPoints--;
	init();
}

void CVortiNinja::init()
{
	state = NINJA_STAND;
	timetillkick = (rnd()%(NINJA_MAX_TIME_TILL_KICK-NINJA_MIN_TIME_TILL_KICK))+NINJA_MIN_TIME_TILL_KICK;
	if(gBehaviorEngine.mDifficulty > NORMAL) timetillkick /= 3;

	animtimer = 0;
	animframe = 0;
	dying = false;
}


bool CVortiNinja::isNearby(CSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
	if(state == NINJA_STAND)
	{
		if (player->getXPosition() < getXPosition()+(8<<STC))
			dir = LEFT;
		else
			dir = RIGHT;
	    
		if(timetillkick)
		{
			// find out if a player is on the same level
			bool onsamelevel = true;

			if ((player->getYPosition() >= getYPosition()-(96<<STC)) &&
				(player->getYDownPos() <= (getYDownPos()+(96<<STC))))
			{
				onsamelevel = true;
			}

			if (onsamelevel)
				timetillkick--;
		}
	}
    }
    
    return true;
}


void CVortiNinja::getTouchedBy(CSpriteObject &theObject)
{
    if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
    {
	if(state != NINJA_DYING)
	{
	    player->kill();
	}	
    }    
}


void CVortiNinja::process()
{
	if (mHealthPoints <= 0 && !dying)
	{
		dying = true;
		dietimer = 0;
        playSound(GameSound::VORT_DIE);
	}


	if (dying)
	{
		if (state == NINJA_STAND)
			state = NINJA_DYING;

		dietimer++;
		if (dietimer > NINJA_DYING_SHOW_TIME)
		{
			mSpriteIdx = NINJA_DEAD_FRAME;
			mIsDead = true;
		}
	}

	switch(state)
	{

	case NINJA_STAND:

		if (!timetillkick)
		{
			state = NINJA_KICK;

			if (rnd()&1)
			{
				// high, short jump
				longjump = false;
				yinertia = -120;
			}
			else
			{
				// low, long jump
				longjump = true;
				yinertia = -30;
			}

		}

		mSpriteIdx = (dir==LEFT) ? NINJA_STAND_LEFT_FRAME : NINJA_STAND_RIGHT_FRAME;
		mSpriteIdx += animframe;

		if (animtimer > NINJA_STAND_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else
			animtimer++;
		break;

	case NINJA_KICK:
		if (!dying)
			mSpriteIdx = (dir==LEFT) ? NINJA_KICK_LEFT_FRAME : NINJA_KICK_RIGHT_FRAME;
		else
			state = NINJA_DYING;

		if (blockedd && yinertia == 0)
		{
			if (!dying)
				init();
			else
				state = NINJA_DYING;

			break;
		}

		if(longjump)
			xinertia = (gBehaviorEngine.mDifficulty > NORMAL) ? 150 : 120;
		else
			xinertia = (gBehaviorEngine.mDifficulty > NORMAL) ? 95 : 75;

		if (dir==LEFT)
			xinertia = -xinertia;

		break;

	case NINJA_DYING:
		mSpriteIdx = NINJA_DYING_FRAME;

		if (dietimer > NINJA_DYING_SHOW_TIME)
		{
			mSpriteIdx = NINJA_DEAD_FRAME;
			mIsDead = true;
		}
		break;
	default: break;
	}
}
