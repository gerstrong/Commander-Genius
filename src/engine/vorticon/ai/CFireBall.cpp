#include "CFireBall.h"
#include "../../../sdl/sound/CSound.h"

// fireball projectile shot out by Vorticon Mother (Ep3)

#define FIREBALL_SPEED   		32
#define FIREBALL_HARD_SPEED		80
#define FIREBALL_ANIM_RATE     20

#define FIREBALL_LEFT_FRAME    57
#define FIREBALL_RIGHT_FRAME   59

#define FIREBALL_OFFSCREEN_KILL_TIME     25

CFireBall::CFireBall(CMap *p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType, size_t byID) :
CRay(p_map, x, y, dir, byType, byID)
{
	animframe = 0;
	animtimer = 0;
	inhibitfall = true;
	blockedl = blockedr = 0;
	canbezapped = 1;
}

void CFireBall::process()
{
	int speed;

	// check if it was shot
	if (HealthPoints <= 0)
	{
		if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
		m_type = OBJ_RAY;
		state = RAY_STATE_SETZAPZOT;
		inhibitfall = 1;
		return;
	}

	// destroy the sound wave if it's been offscreen for a good amount
	// of time. this is to prevent a massive buildup of soundwaves
	// slowly traveling through walls all the way across the level
	// (which can crash the game due to running out of object slots).
	if (!onscreen)
	{
		if (offscreentime > FIREBALL_OFFSCREEN_KILL_TIME)
		{
			exists = false;
			return;
		}
		else offscreentime++;
	}
	else offscreentime = 0;

	// fly through the air
	speed = (mp_Map->m_Difficulty>1) ? FIREBALL_HARD_SPEED : FIREBALL_SPEED;
	if (m_Direction == RIGHT)
	{
		sprite = FIREBALL_RIGHT_FRAME + animframe;
		if (blockedr || blockedl)
		{
			m_type = OBJ_RAY;
			state = RAY_STATE_SETZAPZOT;
			inhibitfall = 1;
			return;
		}
		else moveRight(speed);
	}
	else
	{
		sprite = FIREBALL_LEFT_FRAME + animframe;
		if (blockedr || blockedl)
		{
			m_type = OBJ_RAY;
			state = RAY_STATE_SETZAPZOT;
			inhibitfall = 1;
			return;
		}
		else moveLeft(speed);
	}

	// animation
	if (animtimer > FIREBALL_ANIM_RATE)
	{
		animframe ^= 1;
		animtimer = 0;
	}
	else animtimer++;
}
