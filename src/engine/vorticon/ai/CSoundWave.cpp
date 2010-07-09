#include "CSoundWave.h"

// Sound Wave projectile, emitted by Meeps (ep3)

#define SNDWAVE_SPEED         40
#define SNDWAVE_SPEED_FAST    80

#define SNDWAVE_ANIM_RATE     20

#define SNDWAVE_LEFT_FRAME    128
#define SNDWAVE_RIGHT_FRAME   126

#define SNDWAVE_OFFSCREEN_KILL_TIME     100

CSoundWave::CSoundWave(CMap *p_map, Uint32 x, Uint32 y,
		direction_t dir, object_t byType, size_t byID) :
CRay(p_map, x, y, dir, byType, byID)
{
	//animframe = 0;
	//animtimer = 0;
	inhibitfall = 1;
	needinit = 0;
}

void CSoundWave::process()
{
//	// destroy the sound wave if it's been offscreen for a good amount
//	// of time. this is to prevent a massive buildup of soundwaves
//	// slowly traveling through walls all the way across the level
//	// (which can crash the game due to running out of object slots).
//	if (!onscreen)
//	{
//		if (offscreentime > SNDWAVE_OFFSCREEN_KILL_TIME)
//		{
//			deleteObj(object);
//			return;
//		}
//		else offscreentime++;
//	}
//	else offscreentime = 0;
//
//	// fly through the air
//	int x = getXPosition()>>CSF;
//	if (direction == RIGHT)
//	{
//		sprite = SNDWAVE_RIGHT_FRAME + animframe;
//
//		if (x > static_cast<int>(mp_Map->m_width) )
//			deleteObj(object);
//		else
//		{
//			if (hardmode)
//				moveRight(SNDWAVE_SPEED_FAST);
//			else
//				moveRight(SNDWAVE_SPEED);
//		}
//	}
//	else
//	{
//		sprite = SNDWAVE_LEFT_FRAME + animframe;
//
//		if (x < 2)
//			deleteObj(object);
//		else
//		{
//			if (hardmode)
//				moveLeft(SNDWAVE_SPEED_FAST);
//			else
//				moveLeft(SNDWAVE_SPEED);
//		}
//	}
//
//	// animation
//	if (animtimer > SNDWAVE_ANIM_RATE)
//	{
//		animframe ^= 1;
//		animtimer = 0;
//	}
//	else animtimer++;
}

