#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// Yorp (ep1)

enum
{
YORP_LOOK,     // yorp is "looking around" (waving his eye)
YORP_MOVE,     // yorp is walking either left or right
YORP_STUNNED,  // the yorp is stunned
YORP_DYING,    // the yorp is getting fried!
YORP_DEAD      // they look so sad when they're dead
};

#define YORP_LOOK_TIME  100   // time each frame of YORP_LOOK is shown
#define YORP_STUN_ANIM_TIME  40
#define YORP_WALK_ANIM_TIME  50
#define YORP_WALK_SPEED      3
#define YORP_WALK_ANIM_TIME_FAST  18
#define YORP_WALK_SPEED_FAST      7

#define YORP_NUM_LOOKS  9      // number of times yorp look frame is changed
#define YORP_STUNTIME   28     // YORP_NUM_LOOKS for stun

// for INCREASE_DIFFICULTY
#define YORP_NUM_LOOKS_FAST  3      // number of times yorp look frame is changed
#define YORP_STUNTIME_FAST   12     // YORP_NUM_LOOKS for stun

#define YORP_LOOK_LEFT  49
#define YORP_STAND      50
#define YORP_LOOK_RIGHT 51
#define YORP_WALK_RIGHT 52
#define YORP_WALK_LEFT  54
#define YORP_STUNFRAME  56

#define YORP_JUMP_PROB      40
#define YORP_JUMP_HEIGHT    -7

#define YORPPUSHAMOUNT    24
#define YORPPUSHAMOUNT_FAST    10

#define YORP_DYING_FRAME   58
#define YORP_DEAD_FRAME    59
#define YORP_DIE_TIME      90

void yorp_ai(int o, stLevelControl levelcontrol)
{
int pushamt;
char numlooks;
   // fix where yorps can get stunned, go offscreen, then
   // come back hours later and they're still doing the stun animation
   if (objects[o].wasoffscreen)
   {
     objects[o].wasoffscreen = 0;
     if (objects[o].ai.yorp.state==YORP_STUNNED)
       objects[o].needinit = 1;
   }
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.yorp.state = YORP_LOOK;
     objects[o].ai.yorp.looktimes = YORP_NUM_LOOKS+1;
     objects[o].ai.yorp.timer = 0;
     objects[o].ai.yorp.lookposition = 0;
     objects[o].needinit = 0;
     objects[o].canbezapped = 1;
   }
   // hey after all, dead yorps don't talk. Or do anything
   // but return to the calling procedure.
   if (objects[o].ai.yorp.state==YORP_DEAD) return;

   // code for the yorps to push keen, and code for them to get stunned
       if (objects[o].touchPlayer && objects[o].ai.yorp.state != YORP_STUNNED\
           && objects[o].ai.yorp.state != YORP_DYING)
       {
          if (!player[objects[o].touchedBy].pdie)
          {
            if (player[objects[o].touchedBy].pfalling)
            {  // falling, see if he bonked the yorp on the head
               // this happens if keen's feet are higher than the top
               // half of the yorp
               if ((player[objects[o].touchedBy].y>>CSF)+16 < (objects[o].y>>CSF)+12)
               {
            	  player[objects[o].touchedBy].ppogostick = false; // No pogo, You get it disabled at this point
            	  g_pSound->playStereofromCoord(SOUND_YORP_STUN, PLAY_NOW, objects[o].scrx);
                  objects[o].ai.yorp.state = YORP_STUNNED;
                  objects[o].ai.yorp.looktimes = 0;
                  objects[o].ai.yorp.timer = 0;
                  objects[o].ai.yorp.lookposition = 0;
                  // make the yorp look a little less "soft" by
                  // offering a bit of resistance
                  // (actually, having keen do a small jump)
                  player[objects[o].touchedBy].pjumptime = 0;
                  player[objects[o].touchedBy].pjumpupdecreaserate = 0;
                  player[objects[o].touchedBy].pjumpupspeed = 7;
                  player[objects[o].touchedBy].pjumping = PJUMPUP;
                  player[objects[o].touchedBy].pjumpupspeed_decreasetimer = 0;
                  player[objects[o].touchedBy].pjustjumped = 1;
               }
            }
            else
            {
               if (!levelcontrol.level_done || levelcontrol.level_finished_by != objects[o].touchedBy)
               {
                 // if yorp is moving, also push in direction he's moving
                 // in. this allows walking through a yorp if he is walking
                 // away from Keen
                 if (objects[o].ai.yorp.state==YORP_MOVE)
                 {
                	 g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NOW, objects[o].scrx);
                    if (player[objects[o].touchedBy].pshowdir==objects[o].ai.yorp.movedir)
                      pushamt = 12;
                    else
                      pushamt = YORPPUSHAMOUNT+player[objects[o].touchedBy].pwalking*8;

                    if (objects[o].ai.yorp.movedir==LEFT)
                      goto bump_left;
                    else
                      goto bump_right;
                 }
                 // else, always push keen away from the yorp

                 g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, objects[o].scrx);
                 pushamt = YORPPUSHAMOUNT;
                 if (player[objects[o].touchedBy].x < objects[o].x)
                 {
                   bump_left: ;
                   player[objects[o].touchedBy].playpushed_x = -pushamt;
                   if (levelcontrol.hardmode) player[objects[o].touchedBy].playpushed_x -= YORPPUSHAMOUNT_FAST;
                   player[objects[o].touchedBy].playpushed_decreasetimer = 0;
                   if (!player[objects[o].touchedBy].pjumping)
                   {
                     player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = LEFT;
                   }
                 }
                 else
                 {
                   bump_right: ;
                   player[objects[o].touchedBy].playpushed_x = pushamt;
                   if (levelcontrol.hardmode) player[objects[o].touchedBy].playpushed_x += YORPPUSHAMOUNT_FAST;
                   player[objects[o].touchedBy].playpushed_decreasetimer = 0;
                   if (!player[objects[o].touchedBy].pjumping)
                   {
                     player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = RIGHT;
                   }
                 }
               }
            }
          }
       }

#define YORPDIE_START_INERTIA      -10
#define YORPDIE_MAX_INERTIA         32
#define YORPDIE_INERTIA_DECREASE    2
   // did the yorp get shot?
   if (objects[o].zapped)
   {
      // what'd you kill an innocent yorp for, you emotionless bastard!
      objects[o].ai.yorp.state = YORP_DYING;
      objects[o].ai.yorp.dietimer = 0;
      objects[o].canbezapped = 0;
      objects[o].sprite = YORP_DYING_FRAME;
      objects[o].zapped--;
      objects[o].ai.yorp.yorpdie_inertia_y = YORPDIE_START_INERTIA;
      objects[o].y -= 10;
      objects[o].inhibitfall = 1;
      g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
   }

   switch(objects[o].ai.yorp.state)
   {
     case YORP_DYING:
       objects[o].y += objects[o].ai.yorp.yorpdie_inertia_y;
       if (objects[o].ai.yorp.dietimer>YORPDIE_INERTIA_DECREASE)
         {
           if (objects[o].ai.yorp.yorpdie_inertia_y < YORPDIE_MAX_INERTIA)
              objects[o].ai.yorp.yorpdie_inertia_y++;
           objects[o].ai.yorp.dietimer = 0;
         }
       else objects[o].ai.yorp.dietimer++;
       if (objects[o].ai.yorp.yorpdie_inertia_y >= 0 && objects[o].blockedd)
         {
            objects[o].sprite = YORP_DEAD_FRAME;
            objects[o].ai.yorp.state = YORP_DEAD;
         }
     break;
     case YORP_LOOK:
       if (levelcontrol.hardmode) numlooks = YORP_NUM_LOOKS_FAST; else numlooks = YORP_NUM_LOOKS;
       if (objects[o].ai.yorp.looktimes>numlooks &&\
           objects[o].ai.yorp.timer==YORP_LOOK_TIME-(YORP_LOOK_TIME/4))
       {
          // 75% prob, try to head towards Keen...
             if (player[primaryplayer].x < objects[o].x)
               { objects[o].ai.yorp.movedir = LEFT; }
             else
               { objects[o].ai.yorp.movedir = RIGHT; }
          if (rand()%3==1)
          { // 25% prob go the other way
               objects[o].ai.yorp.movedir ^= 1;
          }
          // unless we're can't go that way
          if (objects[o].blockedl) objects[o].ai.yorp.movedir = RIGHT;
          if (objects[o].blockedr) objects[o].ai.yorp.movedir = LEFT;

          objects[o].ai.yorp.timer = 0;
          objects[o].ai.yorp.walkframe = 0;
          objects[o].ai.yorp.dist_traveled = 0;
          objects[o].ai.yorp.state = YORP_MOVE;
       }

       if (!objects[o].ai.yorp.timer)
       {
         switch(objects[o].ai.yorp.lookposition)
         {
          case 0: objects[o].sprite = YORP_LOOK_LEFT; break;
          case 1: objects[o].sprite = YORP_STAND; break;
          case 2: objects[o].sprite = YORP_LOOK_RIGHT; break;
          case 3: objects[o].sprite = YORP_STAND; break;
         }
         objects[o].ai.yorp.looktimes++;

         if (++objects[o].ai.yorp.lookposition>3)
           objects[o].ai.yorp.lookposition=0;
         objects[o].ai.yorp.timer=YORP_LOOK_TIME;
       } else objects[o].ai.yorp.timer--;
     break;
     case YORP_MOVE:
       #define YORP_LOOK_PROB    1000
       #define YORP_MINTRAVELDIST    1000
       if (objects[o].ai.yorp.dist_traveled > YORP_MINTRAVELDIST)
       {
          if (rand()%YORP_LOOK_PROB==(YORP_LOOK_PROB/2))
          {
              objects[o].ai.yorp.looktimes = 0;
              objects[o].ai.yorp.timer = 0;
              objects[o].ai.yorp.lookposition = 1;
              objects[o].ai.yorp.state = YORP_LOOK;
              break;
          }
       }

       if (objects[o].blockedd)
           if (rand()%YORP_JUMP_PROB==1)
           {
             objects[o].yinertia = YORP_JUMP_HEIGHT - (rand()%3);
             objects[o].y--;
           }

       if (objects[o].ai.yorp.movedir==LEFT)
       {  // yorp is walking left
         objects[o].sprite = YORP_WALK_LEFT + objects[o].ai.yorp.walkframe;
         if (!objects[o].blockedl)
         {
           if (levelcontrol.hardmode)
             objects[o].x -= YORP_WALK_SPEED_FAST;
           else
             objects[o].x -= YORP_WALK_SPEED;

           objects[o].ai.yorp.dist_traveled++;
         }
         else
         {
           objects[o].ai.yorp.looktimes = 4;
           objects[o].ai.yorp.timer = 0;
           objects[o].ai.yorp.lookposition = 1;
           objects[o].ai.yorp.state = YORP_LOOK;
         }
       }
       else
       {  // yorp is walking right
         objects[o].sprite = YORP_WALK_RIGHT + objects[o].ai.yorp.walkframe;
         if (!objects[o].blockedr)
         {
           if (levelcontrol.hardmode)
             objects[o].x += YORP_WALK_SPEED_FAST;
           else
             objects[o].x += YORP_WALK_SPEED;
           objects[o].ai.yorp.dist_traveled++;
         }
         else
         {
           objects[o].ai.yorp.looktimes = 4;
           objects[o].ai.yorp.timer = 0;
           objects[o].ai.yorp.lookposition = 1;
           objects[o].ai.yorp.state = YORP_LOOK;
         }
       }
       // walk animation
       if (objects[o].ai.yorp.timer > YORP_WALK_ANIM_TIME || \
          (objects[o].ai.yorp.timer > YORP_WALK_ANIM_TIME_FAST && levelcontrol.hardmode))
       {
         objects[o].ai.yorp.walkframe ^= 1;
         objects[o].ai.yorp.timer = 0;
       } else objects[o].ai.yorp.timer++;
     break;
   case YORP_STUNNED:
       objects[o].sprite = YORP_STUNFRAME + objects[o].ai.yorp.walkframe;
       if (objects[o].ai.yorp.timer > YORP_STUN_ANIM_TIME)
       {
         if (levelcontrol.hardmode) numlooks = YORP_STUNTIME_FAST; else numlooks = YORP_STUNTIME;
         if (objects[o].ai.yorp.looktimes>numlooks)
         {
           objects[o].ai.yorp.looktimes = 0;
           objects[o].ai.yorp.timer = 0;
           objects[o].ai.yorp.lookposition = 1;
           objects[o].ai.yorp.state = YORP_LOOK;
         } else objects[o].ai.yorp.looktimes++;
         objects[o].ai.yorp.walkframe ^= 1;
         objects[o].ai.yorp.timer = 0;
       } else objects[o].ai.yorp.timer++;
       break;
   }
}
