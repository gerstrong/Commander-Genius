#include "../keen.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// vertically-moving platform (ep3)

#define PLATVERT_MOVE   0
#define PLATVERT_WAIT   1

#define PLATVERT_WAITTIME       150
#define PLATVERT_MOVE_SPD       4

#define PLATVERT_ANIM_RATE      60

#define PLATVERTPUSHAMOUNT      10

void platvert_ai(int o)
{
unsigned int i,p;

   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.platform.animframe = 0;
     objects[o].ai.platform.animtimer = 0;
     objects[o].ai.platform.movedir = UP;
     objects[o].ai.platform.state = PLATVERT_MOVE;

     objects[o].inhibitfall = 1;
     objects[o].needinit = 0;
     objects[o].canbezapped = 1;
     SetAllCanSupportPlayer(o, 1);
     for(i=0;i<numplayers;i++)
     {
       objects[o].ai.platform.kickedplayer[i] = 0;
     }
   }

   // after kicking a player, wait until he falls beneath the platform
   // before turning cansupportplayer back on...just in case we also
   // check for if he stopped falling
   for(p=0;p<numplayers;p++)
   {
     if (objects[o].ai.platform.kickedplayer[p])
     {
       if (player[p].y > objects[o].y || (!player[p].pfalling && !player[p].pjumping))
       {
         objects[o].cansupportplayer[p] = 1;
         objects[o].ai.platform.kickedplayer[p] = 0;
       }
     }
   }

   // push player horizontally
   if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie && player[objects[o].touchedBy].psupportingobject!=o)
   {
     if (objects[o].cansupportplayer[objects[o].touchedBy])
     {
         // if player is standing around minding his own business and we
         // come down on his head, change direction. if player is trying
         // to walk/jump into us horizontally, push him away.
         if (player[objects[o].touchedBy].y > objects[o].y &&
              (player[objects[o].touchedBy].pjumping==PNOJUMP ||
               player[objects[o].touchedBy].pjumping==PPREPAREJUMP ||
               player[objects[o].touchedBy].pjumping==PPREPAREPOGO) &&
             objects[o].ai.platform.movedir==DOWN &&
             !player[objects[o].touchedBy].pfalling)
         {
            // change direction
             objects[o].ai.platform.movedir = UP;
         }
         else
         {
           // push him away
            if (player[objects[o].touchedBy].x < objects[o].x)
            {
               player[objects[o].touchedBy].playpushed_x = -PLATVERTPUSHAMOUNT;
               if (player[objects[o].touchedBy].pinertia_x > 0) player[objects[o].touchedBy].pinertia_x = 0;
               player[objects[o].touchedBy].playpushed_decreasetimer = 0;
            }
            else
            {
               player[objects[o].touchedBy].playpushed_x = PLATVERTPUSHAMOUNT;
               if (player[objects[o].touchedBy].pinertia_x < 0) player[objects[o].touchedBy].pinertia_x = 0;
               player[objects[o].touchedBy].playpushed_decreasetimer = 0;
            }
         }
     }
   }

   objects[o].sprite = OBJ_PLATFORM_DEFSPRITE_EP3 + objects[o].ai.platform.animframe;

   if (objects[o].ai.platform.animtimer > PLATVERT_ANIM_RATE)
   {
     objects[o].ai.platform.animframe ^= 1;
     objects[o].ai.platform.animtimer = 0;
   }
   else objects[o].ai.platform.animtimer++;

   switch(objects[o].ai.platform.state)
   {
   case PLATVERT_MOVE:

       if (objects[o].ai.platform.movedir==UP)
       {
         if (objects[o].blockedu)
         {
           objects[o].ai.platform.movedir = DOWN;
           objects[o].ai.platform.waittimer = 0;
           objects[o].ai.platform.state = PLATVERT_WAIT;
         }
         else
         {
           objects[o].y -= PLATVERT_MOVE_SPD;
           for(i=0;i<numplayers;i++)
           {
              if(player[i].psupportingobject==o && (player[i].pjumping==PNOJUMP||player[i].pjumping==PPREPAREJUMP||player[i].pjumping==PPREPAREPOGO))
              {
                  if (!objects[o].ai.platform.kickedplayer[i])
                  {
                    player[i].y -= PLATVERT_MOVE_SPD;
                  }
                  // kick player off if we're running him into the ceiling
                  if (player[i].blockedu)
                  {
                     objects[o].cansupportplayer[i] = 0;
                     objects[o].ai.platform.kickedplayer[i] = 1;
                  }
              }
           }
         }
       }
       else if (objects[o].ai.platform.movedir==DOWN)
       {
         if (objects[o].blockedd)
           objects[o].ai.platform.movedir = UP;
         else
         {
           objects[o].y += PLATVERT_MOVE_SPD;
           for(i=0;i<numplayers;i++)
           {
              if(player[i].psupportingobject==o && (player[i].pjumping==PNOJUMP||player[i].pjumping==PPREPAREJUMP||player[i].pjumping==PPREPAREPOGO))
              {
                  if (!objects[o].ai.platform.kickedplayer[i])
                    player[i].y += PLATVERT_MOVE_SPD;
              }
           }
         }
       }
   break;
   case PLATVERT_WAIT:
       if (objects[o].ai.platform.waittimer > PLATVERT_WAITTIME)
          objects[o].ai.platform.state = PLATVERT_MOVE;

       else
    	   objects[o].ai.platform.waittimer++;
   break;
   }
}
