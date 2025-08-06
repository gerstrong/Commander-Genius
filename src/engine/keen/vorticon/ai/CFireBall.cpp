#include "CFireBall.h"
#include <base/audio/Audio.h>

// fireball projectile shot out by Vorticon Mother (Ep3)

#define FIREBALL_SPEED   		46
#define FIREBALL_HARD_SPEED		80
#define FIREBALL_ANIM_RATE     20

#define FIREBALL_LEFT_FRAME    57
#define FIREBALL_RIGHT_FRAME   59

#define FIREBALL_OFFSCREEN_KILL_TIME     25

CFireBall::CFireBall(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType, size_t byID) :
CRay(p_map, x, y, dir, CENTER, 0, byType, byID)
{
	animframe = 0;
	animtimer = 0;
	inhibitfall = true;
	blockedl = blockedr = 0;
	canbezapped = 1;
	if(dir == LEFT)
		m_Pos -= GsVec2D<Uint32>(14<<STC,0);

	m_speed = (gBehaviorEngine.mDifficulty>=NORMAL) ? FIREBALL_HARD_SPEED : FIREBALL_SPEED;
	mSpriteIdx = (m_HorDir == RIGHT) ? FIREBALL_RIGHT_FRAME : FIREBALL_LEFT_FRAME;
	performCollisions();
}

void CFireBall::process()
{
	// check if it was shot
	if (mHealthPoints <= 0 && state == RAY_STATE_FLY)
	{
		if (onscreen)
            playSound(GameSound::SHOT_HIT);
		m_type = OBJ_RAY;
		state = RAY_STATE_SETZAPZOT;
		inhibitfall = 1;
		return;
	}

	if(state == RAY_STATE_FLY)
	{
		if (m_HorDir == RIGHT)
			mSpriteIdx = FIREBALL_RIGHT_FRAME + animframe;
		else
			mSpriteIdx = FIREBALL_LEFT_FRAME + animframe;

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
