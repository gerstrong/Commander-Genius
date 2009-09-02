#include "../sdl/sound/CSound.h"

#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

#define MEEP_WALK       0
#define MEEP_SING       1
#define MEEP_DYING      2
#define MEEP_DEAD       3

#define MEEP_WALK_ANIM_RATE     80
#define MEEP_WALK_SPD           2

#define MEEP_SING_PROB          500
#define MEEP_SING_SHOW_TIME     100

#define MEEP_DYING_SHOW_TIME    100

#define MEEP_WALK_RIGHT_FRAME   118
#define MEEP_WALK_LEFT_FRAME    120
#define MEEP_SING_RIGHT_FRAME   122
#define MEEP_SING_LEFT_FRAME    123
#define MEEP_DYING_FRAME        124
#define MEEP_DEAD_FRAME         125

#define SNDWAVE_LEFT_FRAME      128

unsigned int rnd(void);
void bumpplayer(int p, int pushamt, bool solid);

void meep_ai(int o, stLevelControl levelcontrol, CSprite *sprite)
{
int newobject;
int not_about_to_fall;
  if (objects[o].needinit)
  {
    objects[o].ai.meep.state = MEEP_WALK;
    objects[o].ai.meep.animframe = 0;
    objects[o].ai.meep.animtimer = 0;

    if (player[primaryplayer].x > objects[o].x)
      objects[o].ai.meep.dir = RIGHT;
    else
      objects[o].ai.meep.dir = LEFT;

    objects[o].blockedr = objects[o].blockedl = 0;
    objects[o].canbezapped = 1;
    objects[o].needinit = 0;
  }
  if (objects[o].ai.meep.state==MEEP_DEAD) return;

   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie)
   {
      /* don't push the player as he's walking through the exit door */
      if (!levelcontrol.level_done || levelcontrol.level_finished_by != objects[o].touchedBy)
      {
         if (player[objects[o].touchedBy].x < objects[o].x)
        	 bumpplayer(objects[o].touchedBy, -MEEP_WALK_SPD, true);
         else
        	 bumpplayer(objects[o].touchedBy, MEEP_WALK_SPD, true);
      }
   }

  if (objects[o].zapped)
  {
     objects[o].zapped = 0;
     objects[o].canbezapped = 0;
     objects[o].ai.meep.timer = 0;
     objects[o].ai.meep.state = MEEP_DYING;
     g_pSound->playStereofromCoord(SOUND_SHOT_HIT, PLAY_NOW, objects[o].scrx);
  }

  switch(objects[o].ai.meep.state)
  {
   case MEEP_WALK:
     if (rnd()%MEEP_SING_PROB==(MEEP_SING_PROB/2))
     {
       if (objects[o].onscreen)
       {
         objects[o].ai.meep.state = MEEP_SING;
         objects[o].ai.meep.timer = 0;
       }
       else
       {
          // try to get onscreen by heading towards the player
          if (player[primaryplayer].x > objects[o].x)
            objects[o].ai.meep.dir = RIGHT;
          else
            objects[o].ai.meep.dir = LEFT;
       }
     }

     if (objects[o].ai.meep.dir==RIGHT)
     {
       objects[o].sprite = MEEP_WALK_RIGHT_FRAME + objects[o].ai.meep.animframe;

       not_about_to_fall = TileProperty[getmaptileat((objects[o].x>>CSF)+sprite[MEEP_SING_LEFT_FRAME].getWidth(), (objects[o].y>>CSF)+sprite[MEEP_SING_LEFT_FRAME].getHeight())][BUP];

       if (objects[o].blockedr || !not_about_to_fall)
       {
         objects[o].ai.meep.dir = LEFT;
       }
       else
       {
         objects[o].x += MEEP_WALK_SPD;
       }
     }
     else
     {
       objects[o].sprite = MEEP_WALK_LEFT_FRAME + objects[o].ai.meep.animframe;
       not_about_to_fall = TileProperty[getmaptileat((objects[o].x>>CSF)-1, (objects[o].y>>CSF)+sprite[MEEP_SING_LEFT_FRAME].getHeight())][BUP];

       if (objects[o].blockedl || !not_about_to_fall)
       {
         objects[o].ai.meep.dir = RIGHT;
       }
       else
       {
         objects[o].x -= MEEP_WALK_SPD;
       }
     }

     /* walk animation */
     if (objects[o].ai.meep.animtimer > MEEP_WALK_ANIM_RATE)
     {
       objects[o].ai.meep.animframe ^= 1;
       objects[o].ai.meep.animtimer = 0;
     }
     else objects[o].ai.meep.animtimer++;

   break;
   case MEEP_SING:
     if (objects[o].ai.meep.dir==RIGHT)
     {
       objects[o].sprite = MEEP_SING_RIGHT_FRAME;
     }
     else
     {
       objects[o].sprite = MEEP_SING_LEFT_FRAME;
     }

     if (objects[o].ai.meep.timer > MEEP_SING_SHOW_TIME)
     {
        if (objects[o].ai.meep.dir==RIGHT)
        {
            newobject = spawn_object(objects[o].x+(sprite[MEEP_SING_RIGHT_FRAME].getWidth()<<CSF), objects[o].y+(5<<CSF), OBJ_SNDWAVE);
            objects[newobject].ai.ray.direction = RIGHT;
        }
        else
        {
            newobject = spawn_object(objects[o].x-(sprite[SNDWAVE_LEFT_FRAME].getWidth()<<CSF), objects[o].y+(5<<CSF), OBJ_SNDWAVE);
            objects[newobject].ai.ray.direction = LEFT;
        }
        g_pSound->playStereofromCoord(SOUND_MEEP, PLAY_NOW, objects[o].scrx);
        objects[o].ai.meep.state = MEEP_WALK;
     }
     else objects[o].ai.meep.timer++;
     break;
   case MEEP_DYING:
     objects[o].sprite = MEEP_DYING_FRAME;
     if (objects[o].ai.meep.timer > MEEP_DYING_SHOW_TIME)
     {
       objects[o].sprite = MEEP_DEAD_FRAME;
       objects[o].ai.meep.state = MEEP_DEAD;
       objects[o].ai.meep.timer = 0;
     }
     else objects[o].ai.meep.timer++;
   break;
  }
}
