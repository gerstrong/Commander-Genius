#include "../sdl/sound/CSound.h"

#include "../keen.h"
#include "../include/game.h"

#define GARG_LOOK          0
#define GARG_MOVE          1
#define GARG_CHARGE        2
#define GARG_DYING         3
#define GARG_DEAD          4

#define GARG_MINTRAVELDIST  1000
#define GARG_LOOK_PROB      800
#define GARG_WALK_SPEED      2
#define GARG_WALK_ANIM_TIME  50
#define GARG_WALK_SPEED_FAST      5


#define GARG_WALK_ANIM_TIME_FAST  30
#define GARG_CHARGE_SPEED    14
#define GARG_CHARGE_ANIM_TIME  30

#define GARG_LOOK_TIME     70
#define GARG_NUM_LOOKS     3

// amount of time keen must be on same level before garg will charge
#define GARG_SAME_LEVEL_TIME     200

#define GARG_STAND         60
#define GARG_WALK_RIGHT    64
#define GARG_WALK_LEFT     66
#define GARG_DYING_FRAME   68
#define GARG_DEAD_FRAME    69

#define GARGDIE_START_INERTIA      -10
#define GARGDIE_INERTIA_DECREASE    2

char garg_CanWalkLeft(int o);
char garg_CanWalkRight(int o);

void garg_ai(int o, stCloneKeenPlus *pCKP)
{
unsigned int i;

   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.garg.state = GARG_LOOK;
     objects[o].ai.garg.looktimes = GARG_NUM_LOOKS+1;
     objects[o].ai.garg.timer = 0;
     objects[o].ai.garg.lookframe = 0;
     objects[o].ai.garg.keenonsameleveltimer = 0;
     objects[o].ai.garg.about_to_charge = 0;
     objects[o].ai.garg.detectedPlayer = 0;
     objects[o].ai.garg.detectedPlayerIndex = primaryplayer;
     objects[o].canbezapped = 1;
     objects[o].needinit = 0;
   }
   if (objects[o].ai.garg.state==GARG_DEAD) return;

   // kill player on touch
   if (objects[o].ai.garg.state!=GARG_DYING && objects[o].touchPlayer)
   {
     killplayer(objects[o].touchedBy);
   }

   // did the garg get shot?
   if (objects[o].zapped)
   {
      // die, you stupid garg, die!
      objects[o].ai.garg.state = GARG_DYING;
      objects[o].ai.garg.dietimer = 0;
      objects[o].canbezapped = 0;
      objects[o].sprite = GARG_DYING_FRAME;
      objects[o].zapped--;
      objects[o].ai.garg.gargdie_inertia_y = GARGDIE_START_INERTIA;
      objects[o].y -= 10;
      objects[o].inhibitfall = 1;
      g_pSound->playStereofromCoord(SOUND_GARG_DIE, PLAY_NOW, objects[o].scrx);
   }

   switch(objects[o].ai.garg.state)
   {
     case GARG_DYING:
       objects[o].y += objects[o].ai.garg.gargdie_inertia_y;
       if (objects[o].ai.garg.dietimer>GARGDIE_INERTIA_DECREASE)
         {
           objects[o].ai.garg.gargdie_inertia_y++;
           objects[o].ai.garg.dietimer = 0;
         }
       else objects[o].ai.garg.dietimer++;
       if (objects[o].ai.garg.gargdie_inertia_y >= 0 && objects[o].blockedd)
         {
            objects[o].sprite = GARG_DEAD_FRAME;
            objects[o].ai.garg.state = GARG_DEAD;
         }
     break;
     case GARG_LOOK:
       if (objects[o].ai.garg.looktimes>GARG_NUM_LOOKS)
       {
          // try to head towards Keen...
          if (player[objects[o].ai.garg.detectedPlayerIndex].x < objects[o].x)
          {
            objects[o].ai.garg.movedir = LEFT;
          }
          else
          {
            objects[o].ai.garg.movedir = RIGHT;
          }

          if (!objects[o].ai.garg.about_to_charge && rand()%3==1)
          { // 25% prob, go the other way (but always charge towards player)
               objects[o].ai.garg.movedir ^= 1;
          }
          // however if we're blocked on one side we must go the other way
          if (!garg_CanWalkLeft(o)) objects[o].ai.garg.movedir = RIGHT;
          else if (!garg_CanWalkRight(o)) objects[o].ai.garg.movedir = LEFT;

          objects[o].ai.garg.timer = 0;
          objects[o].ai.garg.walkframe = 0;
          objects[o].ai.garg.dist_traveled = 0;
          if (objects[o].ai.garg.about_to_charge)
          {
            objects[o].ai.garg.state = GARG_CHARGE;
            objects[o].ai.garg.about_to_charge = 0;
          }
          else objects[o].ai.garg.state = GARG_MOVE;
       }

       /* look animation */
       if (!objects[o].ai.garg.timer)
       {
         objects[o].sprite = GARG_STAND + objects[o].ai.garg.lookframe;
         objects[o].ai.garg.looktimes++;

         if (++objects[o].ai.garg.lookframe>3)
         {
           objects[o].ai.garg.lookframe=0;
         }
         objects[o].ai.garg.timer = GARG_LOOK_TIME;
       } else objects[o].ai.garg.timer--;
     break;
     case GARG_MOVE:
       // is keen on same level?
         objects[o].ai.garg.detectedPlayer = 0;
         for(i=0;i<numplayers;i++)
         {
           if (player[i].y >= objects[o].y-(8<<CSF))
           {
             if ((player[i].y>>CSF)+sprites[0].ysize <= (objects[o].y>>CSF)+sprites[objects[o].sprite].ysize+16)
             {
               objects[o].ai.garg.detectedPlayer = 1;
               objects[o].ai.garg.detectedPlayerIndex = i;
               break;
             }
           }
         }

         if (objects[o].ai.garg.detectedPlayer)
         {
           objects[o].ai.garg.keenonsameleveltimer++;
           if (objects[o].ai.garg.keenonsameleveltimer > GARG_SAME_LEVEL_TIME)
           { // charge!!
             objects[o].ai.garg.keenonsameleveltimer = 0;
             objects[o].ai.garg.timer = 0;
             objects[o].ai.garg.looktimes = 1;
             objects[o].ai.garg.about_to_charge = 1;
             objects[o].ai.garg.state = GARG_LOOK;
           }
         } else objects[o].ai.garg.keenonsameleveltimer = 0;

       // every now and then go back to look state
       if (objects[o].ai.garg.dist_traveled > GARG_MINTRAVELDIST)
       {
          if (rand()%GARG_LOOK_PROB==(GARG_LOOK_PROB/2))
          {
              objects[o].ai.garg.looktimes = 0;
              objects[o].ai.garg.timer = 0;
              objects[o].ai.garg.state = GARG_LOOK;
              break;
          }
       }

       if (objects[o].ai.garg.movedir==LEFT)
       {  // garg is walking left
         objects[o].sprite = GARG_WALK_LEFT + objects[o].ai.garg.walkframe;

         // do not go left if:
         // * we are blockedl, or
         // * there is empty space for two tiles ahead at floor level,
         //   and there is not a solid block 1-2 tiles ahead at wall level
/*         not_about_to_fall1 = tiles[getmaptileat((objects[o].x>>CSF)-1, (objects[o].y>>CSF)+sprites[GARG_WALK_LEFT].ysize+2)].solidfall;
         not_about_to_fall2 = tiles[getmaptileat((objects[o].x>>CSF)-17, (objects[o].y>>CSF)+sprites[GARG_WALK_LEFT].ysize+2)].solidfall;
         GotoLook = 0;
         if (objects[o].blockedl) GotoLook = 1;
         else if (!(not_about_to_fall1 || not_about_to_fall2))
         {
           blocked_ahead1 = tiles[getmaptileat((objects[o].x>>CSF)-16, (objects[o].y>>CSF)+20)].solidr;
           blocked_ahead2 = tiles[getmaptileat((objects[o].x>>CSF)-28, (objects[o].y>>CSF)+20)].solidr;
           if (!blocked_ahead1 && !blocked_ahead2)
           {
             GotoLook = 1;
           }
         }
         */

         if (garg_CanWalkLeft(o))
         {
           if (pCKP->Control.levelcontrol.hardmode)
             objects[o].x -= GARG_WALK_SPEED_FAST;
           else
             objects[o].x -= GARG_WALK_SPEED;
           objects[o].ai.garg.dist_traveled++;
         }
         else
         {
           objects[o].ai.garg.looktimes = 0;
           objects[o].ai.garg.timer = 0;
           objects[o].ai.garg.state = GARG_LOOK;
         }
       }
       else
       {  // garg is walking right
         objects[o].sprite = GARG_WALK_RIGHT + objects[o].ai.garg.walkframe;
         if (garg_CanWalkRight(o))
         {
           if (pCKP->Control.levelcontrol.hardmode)
             objects[o].x += GARG_WALK_SPEED_FAST;
           else
             objects[o].x += GARG_WALK_SPEED;
           objects[o].ai.garg.dist_traveled++;
         }
         else
         {
           objects[o].ai.garg.looktimes = 0;
           objects[o].ai.garg.timer = 0;
           objects[o].ai.garg.state = GARG_LOOK;
         }
       }

       /* walk animation */
       if (objects[o].ai.garg.timer > GARG_WALK_ANIM_TIME || \
           (objects[o].ai.garg.timer > GARG_WALK_ANIM_TIME && pCKP->Control.levelcontrol.hardmode))
       {
         objects[o].ai.garg.walkframe ^= 1;
         objects[o].ai.garg.timer = 0;
       } else objects[o].ai.garg.timer++;
     break;
     case GARG_CHARGE:

       if (objects[o].ai.garg.movedir==LEFT)
       {  // garg is charging left
         objects[o].sprite = GARG_WALK_LEFT + objects[o].ai.garg.walkframe;
         if (garg_CanWalkLeft(o))
         {
           objects[o].x -= GARG_CHARGE_SPEED;
           objects[o].ai.garg.dist_traveled++;
         }
         else
         {
           objects[o].ai.garg.looktimes = 0;
           objects[o].ai.garg.timer = 0;
           objects[o].ai.garg.state = GARG_LOOK;
         }
       }
       else
       {  // garg is charging right
         objects[o].sprite = GARG_WALK_RIGHT + objects[o].ai.garg.walkframe;
         if (garg_CanWalkRight(o))
         {
           objects[o].x += GARG_CHARGE_SPEED;
           objects[o].ai.garg.dist_traveled++;
         }
         else
         {
           objects[o].ai.garg.looktimes = 0;
           objects[o].ai.garg.timer = 0;
           objects[o].ai.garg.state = GARG_LOOK;
         }
       }

       // walk animation
       if (objects[o].ai.garg.timer > GARG_CHARGE_ANIM_TIME)
       {
         objects[o].ai.garg.walkframe ^= 1;
         objects[o].ai.garg.timer = 0;
       } else objects[o].ai.garg.timer++;
     break;
   }
}

// determines if the garg can walk left
// gargs will not pass a object marker of GARG_STOPPOINT
char garg_CanWalkLeft(int o)
{
int objtile;

  if (objects[o].blockedl) return 0;

  objtile = map.objectlayer[((objects[o].x>>CSF)-1)>>4][((objects[o].y>>CSF)+2)>>4];
  if (objtile == GARG_STOPPOINT) return 0;

  return 1;
}

// determines if the garg can walk right
char garg_CanWalkRight(int o)
{
int objtile;

  if (objects[o].blockedr) return 0;

  objtile = map.objectlayer[((objects[o].x>>CSF)+sprites[GARG_WALK_RIGHT].xsize+1)>>4][((objects[o].y>>CSF)+2)>>4];
  if (objtile == GARG_STOPPOINT) return 0;

  return 1;
}
