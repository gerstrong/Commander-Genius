#include "CFireBall.h"
#include "../../../sdl/sound/CSound.h"

// fireball projectile shot out by Vorticon Mother (Ep3)

#define FIREBALL_SPEED   		32
#define FIREBALL_HARD_SPEED		80
#define FIREBALL_ANIM_RATE     20

#define FIREBALL_LEFT_FRAME    57
#define FIREBALL_RIGHT_FRAME   59

#define FIREBALL_OFFSCREEN_KILL_TIME     25

CFireBall::CFireBall(CMap *p_map) :
CRay(p_map)
{
	animframe = 0;
	animtimer = 0;
	inhibitfall = true;
	blockedl = blockedr = 0;
	canbezapped = 1;
	needinit = 0;
}

void CFireBall::process()
{
	/*int speed;

	// check if it hit keen
	if (touchPlayer)
	{
		killplayer(touchedBy);
		// make a ZAP-ZOT animation
		m_type = OBJ_RAY;
		state = RAY_STATE_SETZAPZOT;
		inhibitfall = 1;
		needinit = 0;
		return;
	}

	// test if it hit a baddie
	std::vector<CObject*>::iterator it_obj = m_Objvect.begin()++;
	for( ; it_obj!=m_Objvect.end() ; it_obj++)
	{
		if (!(*it_obj)->exists || (*it_obj)->m_index==m_index) continue;
		if ( (*it_obj)->m_type==OBJ_FIREBALL) continue;

		if ((*it_obj)->canbezapped || (*it_obj)->m_type==OBJ_RAY)
		{
			if ((*it_obj)->hitdetect(object) && owner != (*it_obj)->m_index)
			{
				m_type = OBJ_RAY;
				state = RAY_STATE_SETZAPZOT;
				inhibitfall = 1;
				needinit = 0;
				(*it_obj)->zapped++;
				(*it_obj)->moveTo(getXPosition(), getYPosition());
				(*it_obj)->zappedbyenemy = 1;
				return;
			}
		}
	}

	// check if it was shot
	if (zapped)
	{
		if (onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, scrx);
		m_type = OBJ_RAY;
		state = RAY_STATE_SETZAPZOT;
		inhibitfall = 1;
		needinit = 0;
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
			deleteObj(object);
			return;
		}
		else offscreentime++;
	}
	else offscreentime = 0;

	// fly through the air
	speed = hard ? FIREBALL_HARD_SPEED : FIREBALL_SPEED;
	if (direction == RIGHT)
	{
		sprite = FIREBALL_RIGHT_FRAME + animframe;
		if (blockedr || blockedl)
		{
			m_type = OBJ_RAY;
			state = RAY_STATE_SETZAPZOT;
			inhibitfall = 1;
			needinit = 0;
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
			needinit = 0;
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
	else animtimer++;*/

}


