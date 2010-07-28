/*
 * CYorp.cpp
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 *
 *  This class describes the AI of the yorp
 */

#include "CYorp.h"
#include "CRay.h"
#include "../../../sdl/sound/CSound.h"
#include "../../../misc.h"

CYorp::CYorp( CMap *p_map, std::vector<CPlayer> &mp_vec_Player,
		Uint32 x, Uint32 y ) :
CObject(p_map,x,y, OBJ_YORP),
m_hardmode(mp_Map->m_Difficulty>1),
m_vec_Player(mp_vec_Player),
state(YORP_LOOK),
looktimes(YORP_NUM_LOOKS+1),
lookposition(0),
timer(0),
dietimer(0),
walkframe(0),
dist_traveled(0),
yorpdie_inertia_y(0),
movedir(LEFT)
{
	m_type = OBJ_YORP;
	sprite = OBJ_YORP_DEFSPRITE;
	canbezapped = true;
}

void CYorp::process()
{
	int pushamt;

	Uint32 x = getXPosition();
	Uint32 y = getYPosition();

	// hey after all, dead yorps don't talk. Or do anything
	// but return to the calling procedure.
	if (state==YORP_DEAD) return;

	if (!hasbeenonscreen) return;

	// did the poor guy get shot?
	if( HealthPoints <= 0 && state != YORP_DYING )
	{
		// what'd you kill an innocent yorp for, you bastard!
		if(!m_hardmode)
		{
		state = YORP_DYING;
		dietimer = 0;
		canbezapped = false;
		sprite = YORP_DYING_FRAME;
		yorpdie_inertia_y = YORPDIE_START_INERTIA;
		moveUp(10);
		inhibitfall = 1;
		g_pSound->playStereofromCoord(SOUND_YORP_DIE, PLAY_NOW, scrx);
		}
		else
		{
			g_pSound->playStereofromCoord(SOUND_YORP_STUN, PLAY_NOW, scrx);
			state = YORP_STUNNED;
			looktimes = 0;
			timer = 0;
			lookposition = 0;
		}
	}

	switch(state)
	{
		case YORP_LOOK:
			processLooking();	break;
		case YORP_MOVE:
			processMoving();	break;
		case YORP_STUNNED:
			processStunned();	break;
		case YORP_DYING:
			processDying();		break;
		default: break;
	}

}

void CYorp::processLooking()
{
	char numlooks = m_hardmode ? YORP_NUM_LOOKS_FAST : YORP_NUM_LOOKS;

	if (looktimes>numlooks && timer==YORP_LOOK_TIME-(YORP_LOOK_TIME/4))
	{
		if (m_vec_Player[0].getXPosition() < getXPosition())
		{ movedir = LEFT; }
		else
		{ movedir = RIGHT; }

		// unless we're can't go that way
		if (blockedl) movedir = RIGHT;
		if (blockedr) movedir = LEFT;

		timer = 0;
		walkframe = 0;
		dist_traveled = 0;
		state = YORP_MOVE;
	}

	if (!timer)
	{
		looktimes++;

		switch(lookposition)
		{
			case 0: sprite = YORP_LOOK_LEFT; break;
			case 1: sprite = YORP_STAND; break;
			case 2: sprite = YORP_LOOK_RIGHT; break;
			case 3: sprite = YORP_STAND; break;
		}
		looktimes++;

		if (++lookposition>3)
			lookposition=0;
		timer=YORP_LOOK_TIME;
	}
	else
		timer--;
}

void CYorp::processMoving()
{
#define YORP_LOOK_PROB    30
#define YORP_MINTRAVELDIST    50
	// looking
	if (dist_traveled > YORP_MINTRAVELDIST)
	{
		// hopping
		if (blockedd && rnd()%YORP_LOOK_PROB==1)
		{
			looktimes = 0;
			timer = 0;
			lookposition = 1;
			state = YORP_LOOK;
			return;
		}
	}

	if (blockedd)
	{
		if (getProbability(YORP_JUMP_PROB))
		{
			yinertia = YORP_JUMP_HEIGHT - (rnd()%3);
			blockedd = false;
		}
	}

	if (movedir==LEFT)
	{  // yorp is walking left
		sprite = YORP_WALK_LEFT + walkframe;
		if (!blockedl)
		{
			moveLeft(m_hardmode ? YORP_WALK_SPEED_FAST : YORP_WALK_SPEED);
			dist_traveled++;
		}
		else
		{
			looktimes = 4;
			timer = 0;
			lookposition = 1;
			state = YORP_LOOK;
		}
	}
	else
	{  // yorp is walking right
		sprite = YORP_WALK_RIGHT + walkframe;
		if (!blockedr)
		{
			moveRight(m_hardmode ? YORP_WALK_SPEED_FAST:YORP_WALK_SPEED);
			dist_traveled++;
		}
		else
		{
			looktimes = 4;
			timer = 0;
			lookposition = 1;
			state = YORP_LOOK;
		}
	}
	// walk animation
	if (timer > YORP_WALK_ANIM_TIME || \
			(timer > YORP_WALK_ANIM_TIME_FAST && m_hardmode))
	{
		walkframe ^= 1;
		timer = 0;
	}
	else
		timer++;
}

void CYorp::processStunned()
{
	sprite = YORP_STUNFRAME + walkframe;
	if (timer > YORP_STUN_ANIM_TIME)
	{
		char numlooks = m_hardmode ? YORP_STUNTIME_FAST : YORP_STUNTIME;
		if (looktimes>numlooks)
		{
			looktimes = 0;
			timer = 0;
			lookposition = 1;
			state = YORP_LOOK;
		} else looktimes++;
		walkframe ^= 1;
		timer = 0;
	}
	else
		timer++;
}

void CYorp::processDying()
{
	moveYDir(yorpdie_inertia_y);
	if (dietimer>YORPDIE_INERTIA_DECREASE)
	{
		if (yorpdie_inertia_y < YORPDIE_MAX_INERTIA)
			yorpdie_inertia_y += (1<<TILE_S);
		dietimer = 0;
	}
	else dietimer++;
	if (yorpdie_inertia_y >= 0 && blockedd)
	{
		sprite = YORP_DEAD_FRAME;
		inhibitfall = 0;
		state = YORP_DEAD;
		dead = 1;
	}
}

void CYorp::getTouchedBy(CObject &theObject)
{
	if(hitdetect(theObject))
	{
		if(theObject.m_type == OBJ_PLAYER)
		{
			CPlayer &tb_player = dynamic_cast<CPlayer&>(theObject);
			// code for the yorps to push keen, and code for them to get stunned
			if (state != YORP_STUNNED && state != YORP_DYING  && !tb_player.pdie)
			{
				if ( tb_player.getYDownPos() < getYDownPos()-(1<<CSF) )
				{
					if (!m_hardmode)
					{
						g_pSound->playStereofromCoord(SOUND_YORP_STUN, PLAY_NOW, scrx);
						state = YORP_STUNNED;
						looktimes = 0;
						timer = 0;
						lookposition = 0;
					}

					// make the yorp look a little less "soft" by
					// offering a bit of resistance
					// (actually, having keen do a small jump)
					tb_player.pjumptime = 0;
					tb_player.pjumpupdecreaserate = 0;
					tb_player.pjumpupspeed = 7;
					tb_player.pjumping = PJUMPUP;
					tb_player.pjustjumped = true;
				}
				else
				{
					// if yorp is moving, also push in direction he's moving
					// in. this allows walking through a yorp if he is walking
					// away from Keen
					tb_player.bump( *this, movedir );
				}
			}
		}
	}
}

CYorp::~CYorp() {
	// TODO Auto-generated destructor stub
}
