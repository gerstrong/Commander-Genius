#include <base/audio/Audio.h>

#include "CVortiKid.h"
#include <base/interface/misc.h>


// Baby Vorticon (the superfast little blue creatures that knock you down)
// (ep 2 & 3)

#define BABY_WALK_SPEED         77

#define BABY_WALK_ANIM_RATE     6

#define BABY_WALK_LEFT_FRAME    48
#define BABY_WALK_RIGHT_FRAME   52
#define BABY_FRY_FRAME          56
#define BABY_DEAD_FRAME         57

#define BABY_JUMP_PROB			100
#define BABY_BOUNCE_PROB		200

#define BABY_FRY_TIME           20
#define BABY_DIE_INERTIA        20

enum baby_jump_style{
BABY_JUMP_BIG, BABY_JUMP_SMALL
};

CVortikid::CVortikid( std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y ) :
CVorticonSpriteObject(p_map, x, y, OBJ_BABY, 0)
{
	bool ep3;
	state = BABY_RUN;
	dir = rand() & 1 ? LEFT : RIGHT;
	walkframe = 0;
	walktimer = 0;
	canbezapped = true;

    const auto diff = gBehaviorEngine.mDifficulty;

	// babies are in ep2 & ep3, but frameset starts one index prior in ep3
	if (gBehaviorEngine.getEpisode()==3) ep3 = true; else ep3 = false;
	mSpriteIdx = BABY_WALK_RIGHT_FRAME - ep3;

    if(diff > NORMAL)
		mHealthPoints++;

    if(diff == EXPERT)
    {
        mSprVar = 1;
        mHealthPoints *= 2;
    }
    else if(diff == NINJA)
    {
        mSprVar = 2;
        mHealthPoints *= 3;
    }
    else if(diff == ELITE)
    {
        mSprVar = 3;
        mHealthPoints *= 4;
    }

}

void CVortikid::getTouchedBy(CSpriteObject &theObject)
{
    if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
    {
    	// touched player
	if (state!=BABY_DYING && !player->pfrozentime)
	{
		player->pDir.x = player->pShowDir.x = dir;
		player->freeze();
	}
    }
}

void CVortikid::process()
{
	bool ep3;

	// babies are in ep2 & ep3, but frameset starts one index prior in ep3
	if (gBehaviorEngine.getEpisode()==3) ep3 = true; else ep3 = false;

	// jumping
	if(blockedd && state == BABY_RUN)
	{
		if(getProbability(BABY_JUMP_PROB))
			baby_jump(BABY_JUMP_SMALL);
	}

	// got hit?
	if (mHealthPoints <= 0 && state != BABY_DYING)
	{
		dietimer = 0;
		state = BABY_DYING;
		dying = true;
		mSpriteIdx = BABY_FRY_FRAME - ep3;
        if (onscreen && !gAudio.isPlaying(int(GameSound::VORT_DIE)))
            playSound(GameSound::VORT_DIE);

        xinertia = (dir == RIGHT) ? BABY_DIE_INERTIA : -BABY_DIE_INERTIA;
	}


	switch(state)
	{
	case BABY_DYING:
		if ((xinertia < 0 && blockedl) ||
				(xinertia > 0 && blockedr))
		{
			xinertia = 0;
		}

		if (xdectimer >= 10)
		{
			if (xinertia < 0)
			{
				xinertia++;
			}
			else if (xinertia > 0)
			{
				xinertia--;
			}

			if (xinertia == 0 && yinertia == 0 &&
				mSpriteIdx == (BABY_DEAD_FRAME-ep3))
			{
				mIsDead = true;
				xinertia = 0;
			}
			xdectimer = 0;
		}
		else xdectimer++;

		if (dietimer > BABY_FRY_TIME)
		{
			mSpriteIdx = BABY_DEAD_FRAME-ep3;
		}
		else dietimer++;
		break;
	case BABY_RUN:
		// run in appropriate direction
		if (dir==RIGHT)
		{ // running right
			mSpriteIdx = BABY_WALK_RIGHT_FRAME + walkframe - ep3;
			if (blockedr)
			{
				dir = LEFT;
				if(getProbability(BABY_BOUNCE_PROB)) baby_jump(BABY_JUMP_BIG);
			}
			else
			{
				xinertia = BABY_WALK_SPEED;
			}
		}
		else
		{ // running left
			mSpriteIdx = BABY_WALK_LEFT_FRAME + walkframe - ep3;
			if (blockedl)
			{
				dir = RIGHT;
				if (getProbability(BABY_BOUNCE_PROB)) baby_jump(BABY_JUMP_BIG);
			}
			else
			{
				xinertia = -BABY_WALK_SPEED;
			}
		}

		// run animation
		if (walktimer > BABY_WALK_ANIM_RATE)
		{
			walkframe++;
			if (walkframe > 3) walkframe = 0;
			walktimer = 0;
		} else walktimer++;
		break;
	default: break;
	}
}

const int BABY_BIGJUMP = 200;
const int BABY_SMALLJUMP = 80;

void CVortikid::baby_jump(int big)
{
	if ((rand()&2)==0) big = 1-big;
	yinertia =  (big==BABY_JUMP_BIG) ? -BABY_BIGJUMP : -BABY_SMALLJUMP ;

	jumpdectimer = 0;
}


