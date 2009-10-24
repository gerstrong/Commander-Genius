#include "../../keen.h"

#include "enemyai.h"

#include "earth.h"

// explosion and earth chunk objects, used for the "earth explodes"
// sequence when you press the switch on a Tantalus Ray in ep2.

#define EXPLODESPRITE           60
#define EXPLODE_ANIM_RATE       80

#define SPACETILE               155

void explosion_ai(int o)
{
  /*if (objects[o].needinit)
  {
    objects[o].ai.ray.animframe = 0;
    objects[o].ai.ray.animtimer = 0;
    objects[o].ai.ray.direction = 1;    // frame forward
    objects[o].inhibitfall = 1;
    objects[o].needinit = 0;
  }

  objects[o].sprite = EXPLODESPRITE + objects[o].ai.ray.animframe;
  if (objects[o].ai.ray.animtimer > EXPLODE_ANIM_RATE)
  {
    if (objects[o].ai.ray.direction==-1 && objects[o].ai.ray.animframe==0)
    {
    	delete_object(o);
    }
    else
    {
      objects[o].ai.ray.animframe += objects[o].ai.ray.direction;
      if (objects[o].ai.ray.direction==1 && objects[o].ai.ray.animframe==3)
      {
        objects[o].ai.ray.direction = -1;
        map_chgtile(((objects[o].x>>CSF)+8)>>4,((objects[o].y>>CSF)+8)>>4,SPACETILE);
      }
    }
    objects[o].ai.ray.animtimer = 0;
  }
  else objects[o].ai.ray.animtimer++;
}

#define EARTHCHUNK_ANIM_RATE       5

#define CHUNKSPD        20
#define HALFCHUNKSPD    (CHUNKSPD/2)
#define BIGCHUNKSPD     14

void earthchunk_ai(int o)
{
  if (objects[o].needinit)
  {
     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
  }
  if (!objects[o].onscreen) delete_object(o);

  switch(objects[o].ai.ray.direction)
  {
   case EC_UPLEFTLEFT:
     objects[o].x -= CHUNKSPD;
     objects[o].y -= HALFCHUNKSPD;
   break;
   case EC_UPUPLEFT:
     objects[o].x -= HALFCHUNKSPD;
     objects[o].y -= CHUNKSPD;
   break;
   case EC_UP:
     objects[o].y -= CHUNKSPD;
   break;
   case EC_UPUPRIGHT:
     objects[o].x += HALFCHUNKSPD;
     objects[o].y -= CHUNKSPD;
   break;
   case EC_UPRIGHTRIGHT:
     objects[o].x += CHUNKSPD;
     objects[o].y -= HALFCHUNKSPD;
   break;
   case EC_DOWNRIGHTRIGHT:
     objects[o].x += CHUNKSPD;
     objects[o].y += HALFCHUNKSPD;
   break;
   case EC_DOWNDOWNRIGHT:
     objects[o].x += HALFCHUNKSPD;
     objects[o].y += CHUNKSPD;
   break;
   case EC_DOWN:
     objects[o].y += CHUNKSPD;
   break;
   case EC_DOWNDOWNLEFT:
     objects[o].x -= HALFCHUNKSPD;
     objects[o].y += CHUNKSPD;
   break;
   case EC_DOWNLEFTLEFT:
     objects[o].x -= CHUNKSPD;
     objects[o].y += HALFCHUNKSPD;
   break;

   case EC_UPLEFT:
     objects[o].x -= BIGCHUNKSPD;
     objects[o].y -= BIGCHUNKSPD;
   break;
   case EC_UPRIGHT:
     objects[o].x += BIGCHUNKSPD;
     objects[o].y -= BIGCHUNKSPD;
   break;
   case EC_DOWNLEFT:
     objects[o].x -= BIGCHUNKSPD;
     objects[o].y += BIGCHUNKSPD;
   break;
   case EC_DOWNRIGHT:
     objects[o].x += BIGCHUNKSPD;
     objects[o].y += BIGCHUNKSPD;
   break;
  }*/
}
