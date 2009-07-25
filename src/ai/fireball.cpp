#include "../keen.h"
#include "ray.h"
#include "../include/game.h"

#include "../include/enemyai.h"

#include "../sdl/sound/CSound.h"

// fireball projectile shot out by Vorticon Mother (Ep3)

#define FIREBALL_SPEED   5

#define FIREBALL_ANIM_RATE     80

#define FIREBALL_LEFT_FRAME    57
#define FIREBALL_RIGHT_FRAME   59

#define FIREBALL_OFFSCREEN_KILL_TIME     100

void fireball_ai(int o, stCloneKeenPlus *pCKP)
{
	int i;
  if (objects[o].needinit)
  {
    objects[o].ai.ray.animframe = 0;
    objects[o].ai.ray.animtimer = 0;
    objects[o].inhibitfall = 1;
    objects[o].blockedl = objects[o].blockedr = 0;
    objects[o].canbezapped = 1;
    objects[o].needinit = 0;
  }

  // test if it hit a baddie
   for(i=1;i<MAX_OBJECTS;i++)
   {
     if (!objects[i].exists || i==o) continue;
     if (objects[o].ai.ray.dontHitEnable)
     {
       if (objects[i].type==objects[o].ai.ray.dontHit) continue;
     }

     if (objects[i].canbezapped && objects[i].type != OBJ_MOTHER)
     {
        if (hitdetect(i, o))
        {
	     objects[o].type = OBJ_RAY;
		 objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
          objects[i].zapped++;
        }
     }
   }

  // check if it hit keen
  if (objects[o].touchPlayer)
  {
     killplayer(objects[o].touchedBy);
     objects[o].type = OBJ_RAY;
     objects[o].ai.ray.state = RAY_STATE_SETZAPZOT;
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
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
      objects[o].exists = 0;
      return;
    }
    else objects[o].ai.ray.offscreentime++;
  }
  else objects[o].ai.ray.offscreentime = 0;

  // fly through the air
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
     else objects[o].x += FIREBALL_SPEED;
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
     else objects[o].x -= FIREBALL_SPEED;
  }

  // animation
  if (objects[o].ai.ray.animtimer > FIREBALL_ANIM_RATE)
  {
    objects[o].ai.ray.animframe ^= 1;
    objects[o].ai.ray.animtimer = 0;
  }
  else objects[o].ai.ray.animtimer++;
}

