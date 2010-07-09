#include "../../../sdl/sound/CSound.h"

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

CVortiNinja::CVortiNinja(CMap *p_map, Uint32 x, Uint32 y, std::vector<CPlayer> &Player) :
CObject(p_map, x, y, OBJ_NINJA),
m_Player(Player)
{
	state = NINJA_STAND;
	timetillkick = (rnd()%(NINJA_MAX_TIME_TILL_KICK-NINJA_MIN_TIME_TILL_KICK))+NINJA_MIN_TIME_TILL_KICK;
	if (mp_Map->m_Difficulty>1) timetillkick /= 3;

	if (m_Player[0].getXPosition() < getXPosition())
		dir = LEFT;
	else
		dir = RIGHT;

	animtimer = 0;
	animframe = 0;
	isdying = 0;
	canbezapped = 1;
	inhibitfall = 0;
	needinit = 0;
}

void CVortiNinja::process()
{
	int onsamelevel;

	if (state==NINJA_DEAD) return;

	if (touchPlayer && !m_Player[touchedBy].pdie && \
			state != NINJA_DYING)
		m_Player[touchedBy].kill();

	if (HealthPoints <= 0)
	{
		isdying = 1;
		dietimer = 0;
		YFrictionRate = 1;
		if (YInertia < 0) YInertia = 0;
		canbezapped = 0;
		g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, scrx);
	}


	if (isdying)
	{
		if (state == NINJA_STAND)
			state = NINJA_DYING;

		dietimer++;
		if (dietimer > NINJA_DYING_SHOW_TIME)
			sprite = NINJA_DEAD_FRAME;
	}

	switch(state)
	{
	case NINJA_STAND:
		if (m_Player[0].getXPosition() < getXPosition()+(8<<STC))
			dir = LEFT;
		else
			dir = RIGHT;

		if (!timetillkick)
		{
			state = NINJA_KICK;
			inhibitfall = 1;

			if (rnd()&1)
			{
				// high, short jump
				XInertia = (mp_Map->m_Difficulty>1) ? 95 : 75;
				YInertia = -120;
				XFrictionTimer = 0;
				YFrictionTimer = 0;
				XFrictionRate = 5;
				YFrictionRate = 1;
			}
			else
			{
				// low, long jump
				XInertia = (mp_Map->m_Difficulty>1) ? 150 : 120;
				YInertia = -30;
				XFrictionTimer = 0;
				YFrictionTimer = 0;
				XFrictionRate = 5;
				YFrictionRate = 1;
			}

			if (dir==LEFT)
				XInertia = -XInertia;
		}
		else
		{
			// find out if a player is on the same level
			onsamelevel = 0;

			std::vector<CPlayer>::iterator it_player = m_Player.begin();
			for( ; it_player != m_Player.end() ; it_player++ )
			{
				if ((it_player->getYPosition() >= getYPosition()-(96<<STC)) &&
					(it_player->getYDownPos() <= (getYDownPos()+(96<<STC))))
				{
					onsamelevel = 1;
					break;
				}
			}

			if (onsamelevel)
				timetillkick--;
		}

		if (dir==LEFT)
			sprite = NINJA_STAND_LEFT_FRAME + animframe;
		else
			sprite = NINJA_STAND_RIGHT_FRAME + animframe;

		if (animtimer > NINJA_STAND_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else
			animtimer++;
		break;
	case NINJA_KICK:
		if (blockedu && YInertia < 0)
			YInertia *= 0.5;

		if (!isdying)
		{
			if (dir==LEFT)
				sprite = NINJA_KICK_LEFT_FRAME;
			else
				sprite = NINJA_KICK_RIGHT_FRAME;
		}
		else
			sprite = NINJA_DYING_FRAME;
		if (YInertia > 0 && blockedd)
		{
			if (!isdying)
				needinit = true;
			else
				state = NINJA_DYING;

			break;
		}
		else
		{
			if ((XInertia > 0 && !blockedr) || \
					(XInertia < 0 && !blockedl))
			{
				if (!isdying)
					moveXDir(XInertia);
			}

			if (YInertia > 0 || !blockedu)
				moveYDir(YInertia);
		}


		if (KickMoveTimer < NINJA_KICK_MOVE_RATE)
		{
			KickMoveTimer++;
			break;
		}
		KickMoveTimer = 0;

		if (XFrictionTimer > XFrictionRate)
		{
			if (XInertia>0)
			{
				if(XInertia-16 < 0)
					XInertia = 0;
				else XInertia-= 16;
			}
			else
			{
				if(XInertia+16 > 0)
					XInertia = 0;
				else XInertia+= 16;
			}

			XFrictionTimer = 0;
		}
		else
			XFrictionTimer++;

		if (YFrictionTimer > YFrictionRate)
		{
			if(!blockedd) YInertia+=16;
			else{
				YInertia=0;
				state = NINJA_STAND;
				needinit = true;
			}
			YFrictionTimer = 0;
		}
		else
			YFrictionTimer++;

		break;
	case NINJA_DYING:
		sprite = NINJA_DYING_FRAME;

		if (dietimer > NINJA_DYING_SHOW_TIME)
		{
			sprite = NINJA_DEAD_FRAME;
			state = NINJA_DEAD;
		}
		break;
	default: break;
	}
}
