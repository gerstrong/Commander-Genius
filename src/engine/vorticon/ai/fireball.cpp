#include "ray.h"

#include "CObjectAI.h"

#include "../../sdl/sound/CSound.h"

// fireball projectile shot out by Vorticon Mother (Ep3)

#define FIREBALL_SPEED   		32
#define FIREBALL_HARD_SPEED		80
#define FIREBALL_ANIM_RATE     20

#define FIREBALL_LEFT_FRAME    57
#define FIREBALL_RIGHT_FRAME   59

#define FIREBALL_OFFSCREEN_KILL_TIME     25


void CObjectAI::fireball_ai(CObject &object, bool hard)
{
	int speed;
	if (object.needinit)
	{
		object.ai.ray.animframe = 0;
		object.ai.ray.animtimer = 0;
		object.inhibitfall = 1;
		object.blockedl = object.blockedr = 0;
		object.canbezapped = 1;
		object.needinit = 0;
	}

	// check if it hit keen
	if (object.touchPlayer)
	{
		killplayer(object.touchedBy);
		// make a ZAP-ZOT animation
		object.m_type = OBJ_RAY;
		object.ai.ray.state = RAY_STATE_SETZAPZOT;
		object.inhibitfall = 1;
		object.needinit = 0;
		return;
	}

	// test if it hit a baddie
	std::vector<CObject>::iterator it_obj = m_Objvect.begin()++;
	for( ; it_obj!=m_Objvect.end() ; it_obj++)
	{
		if (!it_obj->exists || it_obj->m_index==object.m_index) continue;
		if (/*it_obj->m_type==OBJ_RAY ||*/ it_obj->m_type==OBJ_FIREBALL) continue;

		if (it_obj->canbezapped || it_obj->m_type==OBJ_RAY)
		{
			if (it_obj->hitdetect(object) && object.ai.ray.owner != it_obj->m_index)
			{
				object.m_type = OBJ_RAY;
				object.ai.ray.state = RAY_STATE_SETZAPZOT;
				object.inhibitfall = 1;
				object.needinit = 0;
				it_obj->zapped++;
				it_obj->moveTo(object.getXPosition(), object.getYPosition());
				it_obj->zappedbyenemy = 1;
				return;
			}
		}
	}

	// check if it was shot
	if (object.zapped)
	{
		if (object.onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, object.scrx);
		object.m_type = OBJ_RAY;
		object.ai.ray.state = RAY_STATE_SETZAPZOT;
		object.inhibitfall = 1;
		object.needinit = 0;
		return;
	}

	// destroy the sound wave if it's been offscreen for a good amount
	// of time. this is to prevent a massive buildup of soundwaves
	// slowly traveling through walls all the way across the level
	// (which can crash the game due to running out of object slots).
	if (!object.onscreen)
	{
		if (object.ai.ray.offscreentime > FIREBALL_OFFSCREEN_KILL_TIME)
		{
			deleteObj(object);
			return;
		}
		else object.ai.ray.offscreentime++;
	}
	else object.ai.ray.offscreentime = 0;

	// fly through the air
	speed = hard ? FIREBALL_HARD_SPEED : FIREBALL_SPEED;
	if (object.ai.ray.direction == RIGHT)
	{
		object.sprite = FIREBALL_RIGHT_FRAME + object.ai.ray.animframe;
		if (object.blockedr || object.blockedl)
		{
			object.m_type = OBJ_RAY;
			object.ai.ray.state = RAY_STATE_SETZAPZOT;
			object.inhibitfall = 1;
			object.needinit = 0;
			return;
		}
		else object.moveRight(speed);
	}
	else
	{
		object.sprite = FIREBALL_LEFT_FRAME + object.ai.ray.animframe;
		if (object.blockedr || object.blockedl)
		{
			object.m_type = OBJ_RAY;
			object.ai.ray.state = RAY_STATE_SETZAPZOT;
			object.inhibitfall = 1;
			object.needinit = 0;
			return;
		}
		else object.moveLeft(speed);
	}

	// animation
	if (object.ai.ray.animtimer > FIREBALL_ANIM_RATE)
	{
		object.ai.ray.animframe ^= 1;
		object.ai.ray.animtimer = 0;
	}
	else object.ai.ray.animtimer++;
}



