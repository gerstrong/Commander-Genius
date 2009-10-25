#include "../../keen.h"
#include "../../game.h"

#include "enemyai.h"

// horizontally-moving platform (ep2 & 3)

#define PLATFORM_MOVE   0
#define PLATFORM_WAIT   1

#define PLATFORM_WAITTIME       150
#define PLATFORM_MOVE_SPD       3

#define PLATFORM_ANIM_RATE      60

#define PLATFORMPUSHAMOUNT      10

void platform_ai(int o, stLevelControl levelcontrol)
{
/*unsigned int i;

   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.platform.animframe = 0;
     objects[o].ai.platform.animtimer = 0;
     objects[o].ai.platform.movedir = RIGHT;
     objects[o].ai.platform.state = PLATFORM_MOVE;

     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
     objects[o].canbezapped = 1;
     SetAllCanSupportPlayer(o, 1);
   }

   // push player horizontally
   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie && player[objects[o].touchedBy].psupportingobject!=o)
   {
         if (player[objects[o].touchedBy].x < objects[o].x)
         {
            player[objects[o].touchedBy].playpushed_x = -PLATFORMPUSHAMOUNT;
            if (player[objects[o].touchedBy].pinertia_x > 0) player[objects[o].touchedBy].pinertia_x = 0;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
         }
         else
         {
            player[objects[o].touchedBy].playpushed_x = PLATFORMPUSHAMOUNT;
            if (player[objects[o].touchedBy].pinertia_x < 0) player[objects[o].touchedBy].pinertia_x = 0;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
         }
   }

   if (levelcontrol.episode==2)
   {
     objects[o].sprite = OBJ_PLATFORM_DEFSPRITE_EP2 + objects[o].ai.platform.animframe;
   }
   else
   {
     objects[o].sprite = OBJ_PLATFORM_DEFSPRITE_EP3 + objects[o].ai.platform.animframe;
   }

   if (objects[o].ai.platform.animtimer > PLATFORM_ANIM_RATE)
   {
     objects[o].ai.platform.animframe ^= 1;
     objects[o].ai.platform.animtimer = 0;
   }
   else objects[o].ai.platform.animtimer++;

   switch(objects[o].ai.platform.state)
   {
   case PLATFORM_MOVE:

       if (objects[o].ai.platform.movedir==RIGHT)
       {
         if (objects[o].blockedr)
         {
           objects[o].ai.platform.movedir = LEFT;
           objects[o].ai.platform.waittimer = 0;
           objects[o].ai.platform.state = PLATFORM_WAIT;
         }
         else
         {
           objects[o].x += PLATFORM_MOVE_SPD;
           for(i=0;i<numplayers;i++)
           {
              if(player[i].psupportingobject==o && (player[i].pjumping==PNOJUMP||player[i].pjumping==PPREPAREJUMP||player[i].pjumping==PPREPAREPOGO))
              {
                  if (!player[i].blockedr)
                    player[i].x += PLATFORM_MOVE_SPD;
              }
           }
         }
       }
       else if (objects[o].ai.platform.movedir==LEFT)
       {
         if (objects[o].blockedl)
         {
           objects[o].ai.platform.movedir = RIGHT;
           objects[o].ai.platform.waittimer = 0;
           objects[o].ai.platform.state = PLATFORM_WAIT;
         }
         else
         {
           objects[o].x -= PLATFORM_MOVE_SPD;
           for(i=0;i<numplayers;i++)
           {
              if(player[i].psupportingobject==o && (player[i].pjumping==PNOJUMP||player[i].pjumping==PPREPAREJUMP||player[i].pjumping==PPREPAREPOGO))
              {
                  if (!player[i].blockedl)
                    player[i].x -= PLATFORM_MOVE_SPD;
              }
           }
         }
       }
   break;
   case PLATFORM_WAIT:
       if (objects[o].ai.platform.waittimer > PLATFORM_WAITTIME)
       {
          objects[o].ai.platform.state = PLATFORM_MOVE;
       }
       else objects[o].ai.platform.waittimer++;
   break;
   }*/
}

