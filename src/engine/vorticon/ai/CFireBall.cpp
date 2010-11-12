#include "CFireBall.h"
#include "sdl/sound/CSound.h"

// fireball projectile shot out by Vorticon Mother (Ep3)

#define FIREBALL_SPEED   		46
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
	if(dir == LEFT)
		m_Pos -= VectorD2<Uint32>(14<<STC,0);

	m_speed = (mp_Map->m_Difficulty>1) ? FIREBALL_HARD_SPEED : FIREBALL_SPEED;
	sprite = (m_Direction == RIGHT) ? FIREBALL_RIGHT_FRAME : FIREBALL_LEFT_FRAME;
	performCollisions();
}

void CFireBall::process()
{
	// check if it was shot
	if (HealthPoints <= 0 && state == RAY_STATE_FLY)
	{
		if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
		m_type = OBJ_RAY;
		state = RAY_STATE_SETZAPZOT;
		inhibitfall = 1;
		return;
	}

	if(state == RAY_STATE_FLY)
	{
		if (m_Direction == RIGHT)
			sprite = FIREBALL_RIGHT_FRAME + animframe;
		else
			sprite = FIREBALL_LEFT_FRAME + animframe;

		moveinAir();

		// animation
		if (animtimer > FIREBALL_ANIM_RATE)
		{
			animframe ^= 1;
			animtimer = 0;
		}
		else animtimer++;
	}
	else if(state == RAY_STATE_SETZAPZOT)
		setZapped();
	else
		gotZapped();
}
