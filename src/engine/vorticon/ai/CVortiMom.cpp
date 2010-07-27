#include "../../../sdl/sound/CSound.h"
#include "../../../sdl/CVideoDriver.h"
#include "../../../misc.h"

#include "CVortiMom.h"
#include "CFireBall.h"

CVortiMom::CVortiMom(CMap *p_map, Uint32 x, Uint32 y,
		std::vector<CPlayer>& Player,
		std::vector<CObject*>& Object) :
CObject(p_map, x, y, OBJ_MOTHER),
m_Player(Player),
m_Object(Object)
{
	HealthPoints = MOTHER_HP;
	state = MOTHER_WALK;
	animframe = 0;
	animtimer = 0;
	canbezapped = true;

	if (m_Player[0].getXPosition() > getXPosition())
		dir = RIGHT;
	else
		dir = LEFT;

	blockedr = blockedl = 0;
}

void CVortiMom::process()
{
	int prob;

	if (state==MOTHER_DEAD)
	{
		sprite = MOTHER_DEAD_FRAME;
		return;
	}

	if (touchPlayer && !m_Player[touchedBy].pdie)
	{
		// don't push the player as he's walking through the exit door
		if (!m_Player[touchedBy].level_done)
		{
			if (m_Player[touchedBy].getXPosition() < getXPosition())
				m_Player[touchedBy].push(*this);
			else
				m_Player[touchedBy].push(*this);
		}
	}


	switch(state)
	{
	case MOTHER_WALK:

		prob = (mp_Map->m_Difficulty>1) ? MOTHER_SPIT_PROB_HARD : MOTHER_SPIT_PROB;
		if (getProbability(prob))
		{
			if (onscreen)
			{
				if(dir == RIGHT)
					dir = LEFT;
				else
					dir = RIGHT;

				state = MOTHER_SPIT;
				timer = 0;
			}
		}

		if (dir==RIGHT)
		{
			sprite = MOTHER_WALK_RIGHT_FRAME + animframe;

			if (blockedr)
			{
				dir = LEFT;
			}
			else
			{
				moveRight(MOTHER_WALK_SPD);
			}
		}
		else
		{
			sprite = MOTHER_WALK_LEFT_FRAME + animframe;

			if (blockedl)
				dir = RIGHT;
			else
				moveLeft(MOTHER_WALK_SPD);
		}

		// walk animation
		if (animtimer > MOTHER_WALK_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else animtimer++;

		break;

	case MOTHER_SPIT:
		sprite = (dir==RIGHT) ?
				MOTHER_SPIT_RIGHT_FRAME:MOTHER_SPIT_LEFT_FRAME;


		if (timer > MOTHER_SPIT_SHOW_TIME)
		{
			CFireBall *newobject = new CFireBall(mp_Map, getXMidPos()-(3<<STC),
												getYPosition()+(11<<STC), dir,
												OBJ_MOTHER, m_index);
			m_Object.push_back(newobject);

			if (onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, scrx);
			state = MOTHER_WALK;
		}
		else timer++;
		break;
	case MOTHER_HURT:

		sprite = MOTHER_HURT_FRAME;
		if (timer > MOTHER_HURT_SHOW_TIME)
		{
			if (HealthPoints <= 0)
			{
				//sprite = MOTHER_HURT_FRAME;
				state = MOTHER_DEAD;
				canbezapped = 0;
				timer = 0;
				if (onscreen) g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, scrx);
			}
			else
			{
				state = MOTHER_WALK;
				if (m_Player[0].getXPosition() > getXPosition())
					dir = RIGHT;
				else
					dir = LEFT;
			}
		}
		else timer++;
		break;
	default: break;
	}
}

void CVortiMom::getShotByRay()
{
	if( state != MOTHER_HURT && HealthPoints>0 )
	{
		if(HealthPoints>1 && g_pVideoDriver->getSpecialFXConfig())
			blink(10);
		sprite = MOTHER_HURT_FRAME;
		HealthPoints--;
		state = MOTHER_HURT;

	}
}


