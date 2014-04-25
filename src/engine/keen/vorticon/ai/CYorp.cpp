/*
 * CYorp.cpp
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 *
 *  This class describes the AI of the yorp
 */

#include "CYorp.h"
#include "sdl/audio/Audio.h"
#include <base/utils/misc.h>


static signed char numlooks;

CYorp::CYorp( CMap *p_map, Uint32 x, Uint32 y ) :
CVorticonSpriteObject(p_map,x,y, OBJ_YORP),
m_hardmode(g_pBehaviorEngine->mDifficulty > NORMAL),
state(YORP_LOOK),
looktimes(YORP_NUM_LOOKS+1),
lookposition(0),
timer(0),
dietimer(0),
walkframe(0),
dist_traveled(0),
movedir(LEFT)
{
	m_type = OBJ_YORP;
	sprite = OBJ_YORP_DEFSPRITE;
	canbezapped = true;
    deadly = false;

    const Difficulty diff = g_pBehaviorEngine->mDifficulty;

    if(diff > NORMAL)
    {
		mHealthPoints++;
    }

    if(diff > HARD)
    {
        mSprVar = 1;
        deadly = true;
    }
    if(diff > EXPERT)
    {
        mSprVar = 2;
    }

	numlooks = m_hardmode ? YORP_NUM_LOOKS_FAST : YORP_NUM_LOOKS;	
}

void CYorp::process()
{
	if (!hasbeenonscreen) return;

	// did the poor guy get shot?
	if( mHealthPoints <= 0 && state != YORP_DYING )
	{
		// what'd you kill an innocent yorp for, you bastard!
		state = YORP_DYING;
		dying = true;
		dietimer = 0;
		sprite = YORP_DYING_FRAME;
		yinertia = YORPDIE_START_INERTIA;
		playSound(SOUND_YORP_DIE);
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

bool CYorp::isNearby(CVorticonSpriteObject &theObject)
{       
    if( state == YORP_LOOK )
    {
        if (looktimes>numlooks && timer==YORP_LOOK_TIME-(YORP_LOOK_TIME/4))
        {
            if( CPlayer *player = dynamic_cast<CPlayer*>(&theObject) )
            {
                movedir = (player->getXPosition() < getXPosition()) ? LEFT : RIGHT;
            }

            // unless we're can't go that way
            if (blockedl) movedir = RIGHT;
            if (blockedr) movedir = LEFT;

            timer = 0;
            walkframe = 0;
            dist_traveled = 0;
            state = YORP_MOVE;
        }
    }
    return true;
}


void CYorp::processLooking()
{	
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
			xinertia = m_hardmode ? -YORP_WALK_SPEED_FAST : -YORP_WALK_SPEED;
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
			xinertia = m_hardmode ? YORP_WALK_SPEED_FAST : YORP_WALK_SPEED;
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
	if (yinertia >= 0 && blockedd)
	{
		sprite = YORP_DEAD_FRAME;
		dead = true;
	}
}

void CYorp::getTouchedBy(CVorticonSpriteObject &theObject)
{
	if(CPlayer *player = dynamic_cast<CPlayer*>(&theObject))
	{                        
		// code for the yorps to push keen, and code for them to get stunned
		if (state != YORP_STUNNED && state != YORP_DYING  && !player->pdie)
		{            
            const Difficulty diff = g_pBehaviorEngine->mDifficulty;

			if ( player->getYDownPos() < getYDownPos()-(1<<CSF) )
			{
                if (!m_hardmode || (deadly && diff<ELITE) )
				{
					playSound(SOUND_YORP_STUN);
					state = YORP_STUNNED;
					looktimes = 0;
					timer = 0;
					lookposition = 0;
					player->ppogostick = false;
				}

				// make the yorp look a little less "soft" by
				// offering a bit of resistance
				// (actually, having keen do a small jump)
				player->pjumptime = 0;
				player->pjumpupdecreaserate = 0;
				player->pjumpupspeed = 7;
				player->pjumping = PJUMPUP;
				player->pjustjumped = true;
			}
			else
			{
                if(deadly)
                {
                    player->kill();
                    return;
                }

				// if yorp is moving, also push in direction he's moving
				// in. this allows walking through a yorp if he is walking
				// away from Keen
				player->bump( movedir );
			}
		}
	}
}
