#include "../../../sdl/sound/CSound.h"

#include "CVortiKid.h"
#include "../../../misc.h"

// Baby Vorticon (the superfast little blue creatures that knock you down)
// (ep 2 & 3)

enum vort_baby_actions{
	BABY_RUN, BABY_DYING, BABY_DEAD
};

#define BABY_WALK_SPEED         90

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

CVortikid::CVortikid( CMap *p_map, std::vector<CPlayer> &mp_vec_Player,
		Uint32 x, Uint32 y ) :
CObject(p_map, x, y)
{
	state = BABY_RUN;
	dir = rand()&1?LEFT:RIGHT;
	walkframe = 0;
	walktimer = 0;
	inertia_x = 0;
	inertia_y = 0;
	jumpdecrate = 1;
	canbezapped = 1;
	needinit = 0;
	blockedl = blockedr = blockedu = blockedd = 1;

	// babies are in ep2 & ep3, but frameset starts one index prior in ep3
	if (episode==3) ep3 = true; else ep3 = false;
	sprite = BABY_WALK_RIGHT_FRAME - ep3;
}

void CVortikid::process()
{
	bool ep3;

	if (state==BABY_DEAD)
	{
		inhibitfall = 0;
		return;
	}

	// babies are in ep2 & ep3, but frameset starts one index prior in ep3
	if (episode==3) ep3 = true; else ep3 = false;

	// jumping
	if (inertia_y < 0 || !blockedd)
	{
		if (blockedu && inertia_y < 0)
		{
			inertia_y = 0;
		}

		moveYDir(inertia_y);
		inertia_y+=jumpdecrate;

	}
	else
	{
		inertia_y = 0;
		if (state == BABY_RUN)
		{
			if(getProbability(BABY_JUMP_PROB))
				baby_jump(object, BABY_JUMP_SMALL);
		}
	}


	// got hit?
	if (zapped)
	{
		if (zapped > 1 || !hard)
		{  // we're fried!!
			if (state != BABY_DYING)
			{
				dietimer = 0;
				state = BABY_DYING;
				jumpdecrate = 4;
				sprite = BABY_FRY_FRAME - ep3;
				zapped = 0;
				canbezapped = 0;
				if (onscreen && !g_pSound->isPlaying(SOUND_VORT_DIE))
					g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, scrx);

				if (dir == RIGHT)
					inertia_x = BABY_DIE_INERTIA;
				else
					inertia_x = -BABY_DIE_INERTIA;

			}
		}
	}

	// touched player
	if (touchPlayer && state!=BABY_DYING && !m_Player[touchedBy].pfrozentime)
	{
		m_Player[touchedBy].pdir = m_Player[touchedBy].pshowdir = dir;
		m_Player[touchedBy].freeze();
	}

	switch(state)
	{
	case BABY_DYING:
		if ((inertia_x < 0 && blockedl) ||\
				(inertia_x > 0 && blockedr))
		{
			inertia_x = 0;
		}

		moveXDir(inertia_x);
		if (xdectimer >= 10)
		{
			if (inertia_x < 0)
			{
				inertia_x++;
			}
			else if (inertia_x > 0)
			{
				inertia_x--;
			}

			if (inertia_x == 0 &&
					inertia_y == 0 &&
					sprite==(unsigned int)(BABY_DEAD_FRAME-ep3))
			{
				state = BABY_DEAD;
			}
			xdectimer = 0;
		}
		else xdectimer++;

		if (dietimer > BABY_FRY_TIME)
		{
			sprite = BABY_DEAD_FRAME-ep3;
		}
		else dietimer++;
		break;
	case BABY_RUN:
		// run in appropriate direction
		if (dir==RIGHT)
		{ // running right
			sprite = BABY_WALK_RIGHT_FRAME + walkframe - ep3;
			if (blockedr)
			{
				dir = LEFT;
				if(getProbability(BABY_BOUNCE_PROB)) baby_jump(object, BABY_JUMP_BIG);
			}
			else
			{
				moveRight(BABY_WALK_SPEED);
			}
		}
		else
		{ // running left
			sprite = BABY_WALK_LEFT_FRAME + walkframe - ep3;
			if (blockedl)
			{
				dir = RIGHT;
				if (getProbability(BABY_BOUNCE_PROB)) baby_jump(object, BABY_JUMP_BIG);
			}
			else
			{
				moveLeft(BABY_WALK_SPEED);
				inhibitfall = 1;
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
	}
}

#define BABY_BIGJUMP                 200
#define BABY_BIGJUMP_DEC_RATE        4

#define BABY_MIN_SMALLJUMP             140
#define BABY_SMALLJUMP_MIN_DEC_RATE    1
#define BABY_MAX_SMALLJUMP             150
#define BABY_SMALLJUMP_MAX_DEC_RATE    2

void CObjectAI::baby_jump(CObject &object, int big)
{
	if ((rand()&2)==0) big = 1-big;
	if (big==BABY_JUMP_BIG)
	{
		inertia_y = -BABY_BIGJUMP;
		jumpdecrate = BABY_BIGJUMP_DEC_RATE;
	}
	else
	{
		inertia_y = -80;//(rnd()%(BABY_MAX_SMALLJUMP-BABY_MIN_SMALLJUMP))+BABY_MIN_SMALLJUMP;
		jumpdecrate = 4;//(rnd()%(BABY_SMALLJUMP_MAX_DEC_RATE-BABY_SMALLJUMP_MIN_DEC_RATE))+BABY_SMALLJUMP_MIN_DEC_RATE;
	}

	jumpdectimer = 0;
}


