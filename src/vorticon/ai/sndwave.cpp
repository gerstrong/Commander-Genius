#include "CObjectAI.h"

// Sound Wave projectile, emitted by Meeps (ep3)

#define SNDWAVE_SPEED         10
#define SNDWAVE_SPEED_FAST    20

#define SNDWAVE_ANIM_RATE     80

#define SNDWAVE_LEFT_FRAME    128
#define SNDWAVE_RIGHT_FRAME   126

#define SNDWAVE_OFFSCREEN_KILL_TIME     100

void CObjectAI::sndwave_ai(CObject& object, bool hardmode)
{
	if (object.needinit)
	{
		object.ai.ray.animframe = 0;
		object.ai.ray.animtimer = 0;
		object.inhibitfall = 1;
		object.needinit = 0;
	}

	// check if it hit keen
	if (object.touchPlayer)
	{
		killplayer(object.touchedBy);
	}

	// destroy the sound wave if it's been offscreen for a good amount
	// of time. this is to prevent a massive buildup of soundwaves
	// slowly traveling through walls all the way across the level
	// (which can crash the game due to running out of object slots).
	if (!object.onscreen)
	{
		if (object.ai.ray.offscreentime > SNDWAVE_OFFSCREEN_KILL_TIME)
		{
			deleteObj(object);
			return;
		}
		else object.ai.ray.offscreentime++;
	}
	else object.ai.ray.offscreentime = 0;

	// fly through the air
	if (object.ai.ray.direction == RIGHT)
	{
		object.sprite = SNDWAVE_RIGHT_FRAME + object.ai.ray.animframe;

		if (object.x>>CSF > mp_Map->m_width)
			deleteObj(object);
		else
		{
			if (hardmode)
				object.x += SNDWAVE_SPEED_FAST;
			else
				object.x += SNDWAVE_SPEED;
		}
	}
	else
	{
		object.sprite = SNDWAVE_LEFT_FRAME + object.ai.ray.animframe;

		if (object.x>>CSF < 2)
			deleteObj(object);
		else
		{
			if (hardmode)
				object.x -= SNDWAVE_SPEED_FAST;
			else
				object.x -= SNDWAVE_SPEED;
		}
	}

	// animation
	if (object.ai.ray.animtimer > SNDWAVE_ANIM_RATE)
	{
		object.ai.ray.animframe ^= 1;
		object.ai.ray.animtimer = 0;
	}
	else object.ai.ray.animtimer++;
}

