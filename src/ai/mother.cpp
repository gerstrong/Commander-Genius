#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// Vorticon Mother AI (ep3)

#define MOTHER_WALK     0
#define MOTHER_SPIT     1
#define MOTHER_HURT     2
#define MOTHER_DEAD     3

#define MOTHER_WALK_ANIM_RATE     40
#define MOTHER_WALK_SPD           4

#define MOTHER_SPIT_PROB          1000
#define MOTHER_SPIT_SHOW_TIME     100

#define MOTHER_HP      5

#define MOTHER_HURT_SHOW_TIME    100

#define SNDWAVE_LEFT_FRAME    128

#define MOTHER_WALK_LEFT_FRAME	 85
#define MOTHER_WALK_RIGHT_FRAME	 87
#define MOTHER_SPIT_LEFT_FRAME	 89
#define MOTHER_SPIT_RIGHT_FRAME	 90
#define MOTHER_HURT_FRAME        91
#define MOTHER_DEAD_FRAME	 92

void mother_ai(int o, stLevelControl levelcontrol)
{
int newobject;
//int not_about_to_fall;
  if (objects[o].needinit)
  {
    objects[o].ai.mother.state = MOTHER_WALK;
    objects[o].ai.mother.hittimes = 0;
    objects[o].ai.mother.animframe = 0;
    objects[o].ai.mother.animtimer = 0;

    if (player[primaryplayer].x > objects[o].x)
      objects[o].ai.mother.dir = RIGHT;
    else
      objects[o].ai.mother.dir = LEFT;

    objects[o].blockedr = objects[o].blockedl = 0;
    objects[o].canbezapped = 1;
    objects[o].needinit = 0;
  }
  if (objects[o].ai.mother.state==MOTHER_DEAD) return;

   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie)
   {
      /* don't push the player as he's walking through the exit door */
      if (!levelcontrol.level_done || levelcontrol.level_finished_by != objects[o].touchedBy)
      {
         if (player[objects[o].touchedBy].x < objects[o].x)
         {
            player[objects[o].touchedBy].playpushed_x = -MOTHER_WALK_SPD;
            if (player[objects[o].touchedBy].pinertia_x > 0) player[objects[o].touchedBy].pinertia_x = 0;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
         }
         else
         {
            player[objects[o].touchedBy].playpushed_x = MOTHER_WALK_SPD;
            if (player[objects[o].touchedBy].pinertia_x < 0) player[objects[o].touchedBy].pinertia_x = 0;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
         }
      }
   }


  if (objects[o].zapped)
  {
     objects[o].zapped = 0;
     objects[o].ai.mother.hittimes++;
     if (objects[o].ai.mother.state != MOTHER_HURT ||
         objects[o].ai.mother.hittimes < MOTHER_HP)
     {
       objects[o].ai.mother.timer = 0;
       objects[o].ai.mother.state = MOTHER_HURT;
     }
  }

  switch(objects[o].ai.mother.state)
  {
   case MOTHER_WALK:
     if (rand()%MOTHER_SPIT_PROB == (MOTHER_SPIT_PROB/2))
     {
	   if (objects[o].ai.mother.dir == RIGHT) // turn around before spitting
			objects[o].ai.mother.dir = LEFT;
	   else
			objects[o].ai.mother.dir = RIGHT;

	   objects[o].ai.mother.state = MOTHER_SPIT;
       objects[o].ai.mother.timer = 0;
     }

     if (objects[o].ai.mother.dir==RIGHT)
     {
       objects[o].sprite = MOTHER_WALK_RIGHT_FRAME + objects[o].ai.mother.animframe;

       if (objects[o].blockedr)
       {
         objects[o].ai.mother.dir = LEFT;
       }
       else
       {
         objects[o].x += MOTHER_WALK_SPD;
       }
     }
     else
     {
       objects[o].sprite = MOTHER_WALK_LEFT_FRAME + objects[o].ai.mother.animframe;
//       not_about_to_fall = tiles[getmaptileat((objects[o].x>>CSF)-1, (objects[o].y>>CSF)+sprites[MOTHER_WALK_RIGHT_FRAME].ysize)].solidfall;

       if (objects[o].blockedl)// || !not_about_to_fall)
       {
         objects[o].ai.mother.dir = RIGHT;
       }
       else
       {
         objects[o].x -= MOTHER_WALK_SPD;
       }
     }

     /* walk animation */
     if (objects[o].ai.mother.animtimer > MOTHER_WALK_ANIM_RATE)
     {
       objects[o].ai.mother.animframe ^= 1;
       objects[o].ai.mother.animtimer = 0;
     }
     else objects[o].ai.mother.animtimer++;

   break;
   case MOTHER_SPIT:
     if (objects[o].ai.mother.dir==RIGHT)
     {
       objects[o].sprite = MOTHER_SPIT_RIGHT_FRAME;
     }
     else
     {
       objects[o].sprite = MOTHER_SPIT_LEFT_FRAME;
     }

     if (objects[o].ai.mother.timer > MOTHER_SPIT_SHOW_TIME)
     {
        if (objects[o].ai.mother.dir==RIGHT)
        {
            newobject = spawn_object(objects[o].x+(sprites[MOTHER_SPIT_RIGHT_FRAME].xsize<<CSF), objects[o].y+(11<<CSF), OBJ_FIREBALL);
            objects[newobject].ai.ray.direction = RIGHT;
        }
        else
        {
            newobject = spawn_object(objects[o].x-(sprites[MOTHER_SPIT_LEFT_FRAME].xsize<<CSF), objects[o].y+(11<<CSF), OBJ_FIREBALL);
            objects[newobject].ai.ray.direction = LEFT;
        }
        if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_TANK_FIRE, PLAY_NOW, objects[o].scrx);
        objects[o].ai.mother.state = MOTHER_WALK;
     }
     else objects[o].ai.mother.timer++;
     break;
   case MOTHER_HURT:
     objects[o].sprite = MOTHER_HURT_FRAME;
     if (objects[o].ai.mother.timer > MOTHER_HURT_SHOW_TIME)
     {
       if (objects[o].ai.mother.hittimes >= MOTHER_HP)
       {
         objects[o].sprite = MOTHER_DEAD_FRAME;
         objects[o].ai.mother.state = MOTHER_DEAD;
         objects[o].canbezapped = 0;
         objects[o].ai.mother.timer = 0;
         if (objects[o].onscreen) g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
       }
       else
       {
         objects[o].ai.mother.state = MOTHER_WALK;
         if (player[primaryplayer].x > objects[o].x)
           objects[o].ai.mother.dir = RIGHT;
         else
           objects[o].ai.mother.dir = LEFT;
       }
     }
     else objects[o].ai.mother.timer++;
   break;
  }
}
