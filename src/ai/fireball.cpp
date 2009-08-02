#include "../keen.h"
#include "ray.h"
#include "../include/game.h"

#include "../include/enemyai.h"

#include "../sdl/sound/CSound.h"
#include "fireball.h"

// fireball projectile shot out by Vorticon Mother (Ep3)

#define FIREBALL_SPEED   		8
#define FIREBALL_HARD_SPEED		19
#define FIREBALL_ANIM_RATE     80

#define FIREBALL_LEFT_FRAME    57
#define FIREBALL_RIGHT_FRAME   59

#define FIREBALL_OFFSCREEN_KILL_TIME     100

void fireball_ai(int o, bool hard)
{
int i;
int speed;
  if (objects[o].needinit)
  {
    objects[o].ai.ray.animframe = 0;
    objects[o].ai.ray.animtimer = 0;
    objects[o].inhibitfall = 1;
    objects[o].blockedl = objects[o].blockedr = 0;
    objects[o].canbezapped = 1;
    objects[o].needinit = 0;
  }

  // check if it hit keen
  if (objects[o].touchPlayer)
  {
     killplayer(objects[o].touchedBy);
	 // make a ZAP-ZOT animation
     objects[o].type = OBJ_RAY;
     objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
	 return;
  }

  // test if it hit a baddie
	for(i=1;i<highest_objslot;i++)
	{
		if (!objects[i].exists || i==o) continue;
		if (objects[i].type==OBJ_RAY || objects[i].type==OBJ_FIREBALL) continue;

		if (objects[i].canbezapped)
		{
			if (hitdetect(i, o))
			{
				objects[o].type = OBJ_RAY;
				objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
				objects[o].inhibitfall = 1;
				objects[o].needinit = 0;
				objects[i].zapped++;
				objects[i].zapx = objects[o].x;
				objects[i].zapy = objects[o].y;
				objects[i].zapd = objects[o].ai.ray.direction;
				objects[i].zappedbyenemy = 1;
				return;
			}
		}
	}

  // check if it was shot
  if (objects[o].zapped)
  {
     if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
     objects[o].type = OBJ_RAY;
     objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
     return;
  }

  // destroy the sound wave if it's been offscreen for a good amount
  // of time. this is to prevent a massive buildup of soundwaves
  // slowly traveling through walls all the way across the level
  // (which can crash the game due to running out of object slots).
  if (!objects[o].onscreen)
  {
    if (objects[o].ai.ray.offscreentime > FIREBALL_OFFSCREEN_KILL_TIME)
    {
      delete_object(o);
      return;
    }
    else objects[o].ai.ray.offscreentime++;
  }
  else objects[o].ai.ray.offscreentime = 0;

  // fly through the air
  speed = hard ? FIREBALL_HARD_SPEED : FIREBALL_SPEED;
  if (objects[o].ai.ray.direction == RIGHT)
  {
     objects[o].sprite = FIREBALL_RIGHT_FRAME + objects[o].ai.ray.animframe;
     if (objects[o].blockedr || objects[o].blockedl)
     {
        objects[o].type = OBJ_RAY;
        objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
        objects[o].inhibitfall = 1;
        objects[o].needinit = 0;
        return;
     }
     else objects[o].x += speed;
  }
  else
  {
     objects[o].sprite = FIREBALL_LEFT_FRAME + objects[o].ai.ray.animframe;
     if (objects[o].blockedr || objects[o].blockedl)
     {
        objects[o].type = OBJ_RAY;
        objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
        objects[o].inhibitfall = 1;
        objects[o].needinit = 0;
        return;
     }
     else objects[o].x -= speed;
  }

  // animation
  if (objects[o].ai.ray.animtimer > FIREBALL_ANIM_RATE)
  {
    objects[o].ai.ray.animframe ^= 1;
    objects[o].ai.ray.animtimer = 0;
  }
  else objects[o].ai.ray.animtimer++;
}



