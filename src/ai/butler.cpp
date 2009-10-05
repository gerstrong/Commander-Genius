#include "../keen.h"

#include "../include/game.h"

#include "../include/enemyai.h"

#include "../sdl/sound/CSound.h"
#include "../graphics/CGfxEngine.h"

// AI for "butler" robot (ep1)
#define BUTLER_TURN     0
#define BUTLER_WALK     1

#define BUTLER_WALK_SPEED        6
#define BUTLER_WALK_SPEED_FAST   10
#define BUTLER_WALK_ANIM_TIME    35
#define BUTLER_WALK_ANIM_TIME_FAST    25
#define BUTLER_TURN_TIME         50

#define BUTLERPUSHAMOUNT         22
#define BUTLERPUSHAMOUNTFAST     15

// distance in pixels butler should look ahead to avoid falling
// off an edge
#define BUTLER_LOOK_AHEAD_DIST   1

// frames
#define BUTLER_WALK_LEFT_FRAME   92
#define BUTLER_WALK_RIGHT_FRAME  88
#define BUTLER_TURNLEFT_FRAME    96
#define BUTLER_TURNRIGHT_FRAME   97

void butler_ai(int o, bool hardmode)
{
/*char not_about_to_fall;
Uint16 butler_height, butler_width;
CSprite *sprite = g_pGfxEngine->Sprite[BUTLER_WALK_LEFT_FRAME];

   if (objects[o].needinit)
   {
     objects[o].ai.butler.state = BUTLER_WALK;
     objects[o].ai.butler.movedir = RIGHT;
     objects[o].ai.butler.animtimer = 0;
     objects[o].canbezapped = 1;  // will stop bullets but are not harmed
     objects[o].needinit = 0;
     objects[o].y -= 8;
   }
   // push keen
     if (objects[o].touchPlayer && !player[objects[o].touchedBy].pdie)
     {
    	 if(!((player[primaryplayer].pdir == objects[o].ai.butler.movedir) && (player[primaryplayer].pwalking)))
    	 {
    		 g_pSound->playStereofromCoord(SOUND_YORP_BUMP, PLAY_NORESTART, objects[o].scrx);

          short butlerpushamount;

          butlerpushamount = BUTLERPUSHAMOUNT;

          if(player[primaryplayer].pwalking) butlerpushamount = 3*BUTLERPUSHAMOUNT/2;

          if (player[primaryplayer].x < objects[o].x)
          {
            player[objects[o].touchedBy].playpushed_x = -butlerpushamount;
            if (hardmode) player[objects[o].touchedBy].playpushed_x -= BUTLERPUSHAMOUNTFAST;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
            player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = LEFT;
          }
          else
          {
            player[objects[o].touchedBy].playpushed_x = butlerpushamount;
            if (hardmode) player[objects[o].touchedBy].playpushed_x += BUTLERPUSHAMOUNTFAST;
            player[objects[o].touchedBy].playpushed_decreasetimer = 0;
            player[objects[o].touchedBy].pdir = player[objects[o].touchedBy].pshowdir = RIGHT;
          }
    	 }
     }

   switch(objects[o].ai.butler.state)
   {
    case BUTLER_TURN:
       if (objects[o].ai.butler.timer > BUTLER_TURN_TIME)
       {
         objects[o].ai.butler.movedir ^= 1;
         objects[o].ai.butler.animtimer = 0;
         objects[o].ai.butler.state = BUTLER_WALK;
       } else objects[o].ai.butler.timer++;
    break;
    case BUTLER_WALK:
    	butler_height = sprite->getHeight();
    	butler_width = sprite->getWidth();
       if (objects[o].ai.butler.movedir==LEFT)
       {  // move left

         not_about_to_fall = TileProperty[getmaptileat((objects[o].x>>CSF)-BUTLER_LOOK_AHEAD_DIST, (objects[o].y>>CSF)+butler_height)][BUP];
         objects[o].sprite = BUTLER_WALK_LEFT_FRAME + objects[o].ai.butler.frame;
         if (!objects[o].blockedl && not_about_to_fall)
         {
            if (hardmode)
              objects[o].x -= BUTLER_WALK_SPEED_FAST;
            else
              objects[o].x -= BUTLER_WALK_SPEED;
         }
         else
         {
           objects[o].sprite = BUTLER_TURNRIGHT_FRAME;
           objects[o].ai.butler.frame = 0;
           objects[o].ai.butler.timer = 0;
           objects[o].ai.butler.animtimer = 0;
           objects[o].ai.butler.state = BUTLER_TURN;
         }
       }
       else
       {  // move right

     	 not_about_to_fall = TileProperty[getmaptileat((objects[o].x>>CSF)+butler_width+BUTLER_LOOK_AHEAD_DIST, (objects[o].y>>CSF)+butler_height)][BUP];
         objects[o].sprite = BUTLER_WALK_RIGHT_FRAME + objects[o].ai.butler.frame;
         if (!objects[o].blockedr && not_about_to_fall)
         {
            if (hardmode)
              objects[o].x += BUTLER_WALK_SPEED_FAST;
            else
              objects[o].x += BUTLER_WALK_SPEED;
         }
         else
         {
           objects[o].sprite = BUTLER_TURNLEFT_FRAME;
           objects[o].ai.butler.frame = 0;
           objects[o].ai.butler.timer = 0;
           objects[o].ai.butler.animtimer = 0;
           objects[o].ai.butler.state = BUTLER_TURN;
         }
       }
       // walk animation
       if (objects[o].ai.butler.animtimer > BUTLER_WALK_ANIM_TIME || \
           (objects[o].ai.butler.animtimer > BUTLER_WALK_ANIM_TIME_FAST && hardmode))
       {
         if (objects[o].ai.butler.frame>=3) objects[o].ai.butler.frame=0;
                                     else objects[o].ai.butler.frame++;
         objects[o].ai.butler.animtimer = 0;
       } else objects[o].ai.butler.animtimer++;
    break;
   }*/
}
