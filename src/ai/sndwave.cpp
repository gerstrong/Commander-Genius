#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// Sound Wave projectile, emitted by Meeps (ep3)

#define SNDWAVE_SPEED         10
#define SNDWAVE_SPEED_FAST    20

#define SNDWAVE_ANIM_RATE     80

#define SNDWAVE_LEFT_FRAME    128
#define SNDWAVE_RIGHT_FRAME   126

#define SNDWAVE_OFFSCREEN_KILL_TIME     100

void sndwave_ai(int o, stCloneKeenPlus *pCKP)
{
//int i;
  if (objects[o].needinit)
  {
    objects[o].ai.ray.animframe = 0;
    objects[o].ai.ray.animtimer = 0;
    objects[o].inhibitfall = 1;
    objects[o].needinit = 0;
  }

  // check if it hit keen
  if (objects[o].touchPlayer)
  {
     killplayer(objects[o].touchedBy, pCKP);
  }

  // destroy the sound wave if it's been offscreen for a good amount
  // of time. this is to prevent a massive buildup of soundwaves
  // slowly traveling through walls all the way across the level
  // (which can crash the game due to running out of object slots).
  if (!objects[o].onscreen)
  {
    if (objects[o].ai.ray.offscreentime > SNDWAVE_OFFSCREEN_KILL_TIME)
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
     objects[o].sprite = SNDWAVE_RIGHT_FRAME + objects[o].ai.ray.animframe;
     if (objects[o].x>>CSF>>4 > map.xsize-2)
     {
        objects[o].exists = 0;
     }
     else
     {
        if (pCKP->Control.levelcontrol.hardmode)
          objects[o].x += SNDWAVE_SPEED_FAST;
        else
          objects[o].x += SNDWAVE_SPEED;
     }
  }
  else
  {
     objects[o].sprite = SNDWAVE_LEFT_FRAME + objects[o].ai.ray.animframe;
     if (objects[o].x>>CSF>>4 < 2)
     {
        objects[o].exists = 0;
     }
     else
     {
        if (pCKP->Control.levelcontrol.hardmode)
          objects[o].x -= SNDWAVE_SPEED_FAST;
        else
          objects[o].x -= SNDWAVE_SPEED;
     }
  }

  // animation
  if (objects[o].ai.ray.animtimer > SNDWAVE_ANIM_RATE)
  {
    objects[o].ai.ray.animframe ^= 1;
    objects[o].ai.ray.animtimer = 0;
  }
  else objects[o].ai.ray.animtimer++;
}

