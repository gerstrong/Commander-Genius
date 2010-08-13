#include "sdl/sound/CSound.h"

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
longjump(false),
m_Player(Player)
{
	canbezapped = true;
	HealthPoints = 4;
	init();
}

void CVortiNinja::init()
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
	dying = false;
}

void CVortiNinja::process()
{
	bool onsamelevel;

	if (touchPlayer && !m_Player[touchedBy].pdie && \
			state != NINJA_DYING)
		m_Player[touchedBy].kill();

	if (HealthPoints <= 0 && !dying)
	{
		dying = true;
		dietimer = 0;
		g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, scrx);
	}


	if (dying)
	{
		if (state == NINJA_STAND)
			state = NINJA_DYING;

		dietimer++;
		if (dietimer > NINJA_DYING_SHOW_TIME)
		{
			sprite = NINJA_DEAD_FRAME;
			dead = true;
		}
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
		else
		{
			// find out if a player is on the same level
			onsamelevel = false;

			std::vector<CPlayer>::iterator it_player = m_Player.begin();
			for( ; it_player != m_Player.end() ; it_player++ )
			{
				if ((it_player->getYPosition() >= getYPosition()-(96<<STC)) &&
					(it_player->getYDownPos() <= (getYDownPos()+(96<<STC))))
				{
					onsamelevel = true;
					break;
				}
			}

			if (onsamelevel)
				timetillkick--;
		}

		sprite = (dir==LEFT) ? NINJA_STAND_LEFT_FRAME : NINJA_STAND_RIGHT_FRAME;
		sprite += animframe;

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
			sprite = (dir==LEFT) ? NINJA_KICK_LEFT_FRAME : NINJA_KICK_RIGHT_FRAME;
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
			xinertia = (mp_Map->m_Difficulty>1) ? 150 : 120;
		else
			xinertia = (mp_Map->m_Difficulty>1) ? 95 : 75;

		if (dir==LEFT)
			xinertia = -xinertia;

		break;

	case NINJA_DYING:
		sprite = NINJA_DYING_FRAME;

		if (dietimer > NINJA_DYING_SHOW_TIME)
		{
			sprite = NINJA_DEAD_FRAME;
			dead = true;
		}
		break;
	default: break;
	}
}
