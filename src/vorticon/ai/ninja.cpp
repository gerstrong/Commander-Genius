#include "../../sdl/sound/CSound.h"
#include "../../keen.h"
#include "../../include/game.h"

#include "enemyai.h"

// Ninja AI (the black, bear-like karate-kicking creature in ep3)
enum ninja_actions{
NINJA_STAND,
NINJA_KICK,
NINJA_DYING,
NINJA_DEAD
};

#define NINJA_STAND_ANIM_RATE          100
#define NINJA_DYING_SHOW_TIME          100

#define NINJA_MIN_TIME_TILL_KICK       300
#define NINJA_MAX_TIME_TILL_KICK       500

#define NINJA_KICK_MOVE_RATE           1

#define NINJA_STAND_LEFT_FRAME         77
#define NINJA_STAND_RIGHT_FRAME        79
#define NINJA_KICK_LEFT_FRAME          81
#define NINJA_KICK_RIGHT_FRAME         82
#define NINJA_DYING_FRAME              83
#define NINJA_DEAD_FRAME               84

#define Sprite g_pGfxEngine->Sprite

unsigned int rnd(void);

void ninja_ai(int o, bool hardmode)
{
/*unsigned int i;
int onsamelevel;

  if (objects[o].needinit)
  {
    objects[o].ai.ninja.state = NINJA_STAND;
    objects[o].ai.ninja.timetillkick = (rand()%(NINJA_MAX_TIME_TILL_KICK-NINJA_MIN_TIME_TILL_KICK))+NINJA_MIN_TIME_TILL_KICK;
    if (hardmode) objects[o].ai.ninja.timetillkick /= 3;

    if (player[primaryplayer].x < objects[o].x)
       objects[o].ai.ninja.dir = LEFT;
    else
       objects[o].ai.ninja.dir = RIGHT;

    objects[o].ai.ninja.animtimer = 0;
    objects[o].ai.ninja.animframe = 0;
    objects[o].ai.ninja.isdying = 0;
    objects[o].canbezapped = 1;
    objects[o].inhibitfall = 0;
    objects[o].needinit = 0;
  }
  if (objects[o].ai.ninja.state==NINJA_DEAD) return;

  if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie && \
      objects[o].ai.ninja.state != NINJA_DYING)
     killplayer(objects[o].touchedBy);

  if (objects[o].zapped >= 4)
  {
    objects[o].ai.ninja.isdying = 1;
    objects[o].ai.ninja.dietimer = 0;
    objects[o].ai.ninja.YFrictionRate = 1;
    if (objects[o].ai.ninja.YInertia < 0) objects[o].ai.ninja.YInertia = 0;
    objects[o].zapped = 0;
    objects[o].canbezapped = 0;
    g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
  }


  if (objects[o].ai.ninja.isdying)
  {
    if (objects[o].ai.ninja.state == NINJA_STAND)
      objects[o].ai.ninja.state = NINJA_DYING;

    objects[o].ai.ninja.dietimer++;
    if (objects[o].ai.ninja.dietimer > NINJA_DYING_SHOW_TIME)
      objects[o].sprite = NINJA_DEAD_FRAME;
  }

  switch(objects[o].ai.ninja.state)
  {
   case NINJA_STAND:
     if (player[primaryplayer].x < objects[o].x+(8<<CSF))
       objects[o].ai.ninja.dir = LEFT;
     else
       objects[o].ai.ninja.dir = RIGHT;

     if (!objects[o].ai.ninja.timetillkick)
     {
       objects[o].ai.ninja.state = NINJA_KICK;
       objects[o].inhibitfall = 1;

       if (rnd()&1)
       {
         // high, short jump
         objects[o].ai.ninja.XInertia = 25;
         objects[o].ai.ninja.YInertia = -30;
         objects[o].ai.ninja.XFrictionTimer = 0;
         objects[o].ai.ninja.YFrictionTimer = 0;
         objects[o].ai.ninja.XFrictionRate = 10;
         objects[o].ai.ninja.YFrictionRate = 2;
       }
       else
       {
         // low, long jump
         objects[o].ai.ninja.XInertia = 42;
         objects[o].ai.ninja.YInertia = -15;
         objects[o].ai.ninja.XFrictionTimer = 0;
         objects[o].ai.ninja.YFrictionTimer = 0;
         objects[o].ai.ninja.XFrictionRate = 10;
         objects[o].ai.ninja.YFrictionRate = 5;
       }

       if (objects[o].ai.ninja.dir==LEFT)
         objects[o].ai.ninja.XInertia = -objects[o].ai.ninja.XInertia;
     }
     else
     {
        // find out if a player is on the same level
        onsamelevel = 0;
        for(i=0;i<numplayers;i++)
        {
          if (player[i].isPlaying)
          {
            if ((player[i].y >= objects[o].y-(96<<CSF)) && ((player[i].y>>CSF)+Sprite[0]->getHeight() <= (objects[o].y>>CSF)+Sprite[objects[o].sprite]->getHeight()+96))
            {
              onsamelevel = 1;
              break;
            }
          }
        }

        if (onsamelevel)
          objects[o].ai.ninja.timetillkick--;
     }

     if (objects[o].ai.ninja.dir==LEFT)
       objects[o].sprite = NINJA_STAND_LEFT_FRAME + objects[o].ai.ninja.animframe;
     else
       objects[o].sprite = NINJA_STAND_RIGHT_FRAME + objects[o].ai.ninja.animframe;

     if (objects[o].ai.ninja.animtimer > NINJA_STAND_ANIM_RATE)
     {
       objects[o].ai.ninja.animframe ^= 1;
       objects[o].ai.ninja.animtimer = 0;
     }
     else
    	 objects[o].ai.ninja.animtimer++;
     break;
   case NINJA_KICK:
     if (objects[o].blockedu && objects[o].ai.ninja.YInertia < 0)
    	   objects[o].ai.ninja.YInertia *= 0.5;

     if (!objects[o].ai.ninja.isdying)
	 {
		   if (objects[o].ai.ninja.dir==LEFT)
		    objects[o].sprite = NINJA_KICK_LEFT_FRAME;
		   else
		    objects[o].sprite = NINJA_KICK_RIGHT_FRAME;
     }
     else
        objects[o].sprite = NINJA_DYING_FRAME;

     if (objects[o].ai.ninja.KickMoveTimer < NINJA_KICK_MOVE_RATE)
     {
       objects[o].ai.ninja.KickMoveTimer++;
       break;
     }
     objects[o].ai.ninja.KickMoveTimer = 0;

     if (objects[o].ai.ninja.YInertia > 0 && objects[o].blockedd)
     {
       if (!objects[o].ai.ninja.isdying)
         objects[o].needinit = 1;
       else
         objects[o].ai.ninja.state = NINJA_DYING;

       break;
     }
     else
     {
       if ((objects[o].ai.ninja.XInertia > 0 && !objects[o].blockedr) || \
           (objects[o].ai.ninja.XInertia < 0 && !objects[o].blockedl))
       {
         if (!objects[o].ai.ninja.isdying)
           objects[o].x += objects[o].ai.ninja.XInertia;
       }

       if (objects[o].ai.ninja.YInertia > 0 || !objects[o].blockedu)
         objects[o].y += objects[o].ai.ninja.YInertia;
     }

     if (objects[o].ai.ninja.XFrictionTimer > objects[o].ai.ninja.XFrictionRate)
     {
       if (objects[o].ai.ninja.XInertia>0)
           objects[o].ai.ninja.XInertia--;
       else
           objects[o].ai.ninja.XInertia++;

       objects[o].ai.ninja.XFrictionTimer = 0;
     }
     else
    	 objects[o].ai.ninja.XFrictionTimer++;

     if (objects[o].ai.ninja.YFrictionTimer > objects[o].ai.ninja.YFrictionRate)
     {
       objects[o].ai.ninja.YInertia++;
       objects[o].ai.ninja.YFrictionTimer = 0;
     }
     else
    	 objects[o].ai.ninja.YFrictionTimer++;

     break;
   case NINJA_DYING:
     objects[o].sprite = NINJA_DYING_FRAME;

     if (objects[o].ai.ninja.dietimer > NINJA_DYING_SHOW_TIME)
     {
       objects[o].sprite = NINJA_DEAD_FRAME;
       objects[o].ai.ninja.state = NINJA_DEAD;
     }
     break;
  }*/
}
