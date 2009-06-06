#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "vort.h"
#include "../include/game.h"

#include "../include/enemyai.h"

// The rope holding the stone which kills the final Vorticon (ep1)
// (Oh shit, was that a spoiler? I'm sorry)

// ;)

void rope_movestone(int o);
void killboss(int o);
#define ROPE_IDLE            0
#define ROPE_DROP            1

#define STONE_WIDTH          9
#define STONE_HEIGHT         2

#define STONE_DROP_RATE      100
#define STONE_DROP_TIMES     4

#define ROPE_SPRITE          114

void rope_ai(int o)
{
int i;
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.rope.state = ROPE_IDLE;
     objects[o].canbezapped = 1;
     objects[o].inhibitfall = 1;
     objects[o].sprite = ROPE_SPRITE;
     // find the vorticon commander
     for(i=0;i<MAX_OBJECTS;i++)
     {
        if (objects[i].type == OBJ_VORT)
        {
          objects[o].ai.rope.vortboss = i;
          break;
        }
     }
     objects[o].needinit = 0;
   }

   switch(objects[o].ai.rope.state)
   {
     case ROPE_IDLE:
       if (objects[o].zapped)
       {  // time to drop the stone
         objects[o].ai.rope.state = ROPE_DROP;
         objects[o].ai.rope.droptimer = STONE_DROP_RATE;
         objects[o].ai.rope.droptimes = 0;
         // get upper left corner of the stone
         objects[o].ai.rope.stoneX = (objects[o].x >> CSF >> 4) - 4;
         objects[o].ai.rope.stoneY = (objects[o].y >> CSF >> 4) + 1;
         // hide the rope
         objects[o].sprite = BlankSprite;
       }
     break;
     case ROPE_DROP:
       if (objects[o].ai.rope.droptimer > STONE_DROP_RATE)
       {
         rope_movestone(o);
         objects[o].ai.rope.droptimer = 0;

         objects[o].ai.rope.droptimes++;
         if (objects[o].ai.rope.droptimes >= STONE_DROP_TIMES)
         {
            // just to make sure the boss is dead
//            killboss(o);
            objects[o].exists = 0;
         }
       }
       else objects[o].ai.rope.droptimer++;
     break;
   }
}

void rope_movestone(int o)
{
int xa,ya;
int x,y;
unsigned int xpix,ypix,vb;
     xa = objects[o].ai.rope.stoneX;
     ya = objects[o].ai.rope.stoneY;

     // move the stone down one space
     for(y=STONE_HEIGHT;y>0;y--)
     {
       for(x=0;x<STONE_WIDTH;x++)
       {
          map_chgtile(x+xa,y+ya,map.mapdata[x+xa][y+ya-1]);
          // if the stone has hit the vorticon commander, kill him
          xpix = (x+xa)<<4<<CSF; ypix = (y+ya)<<4<<CSF;
          vb = objects[o].ai.rope.vortboss;
          if (xpix <= objects[vb].x && xpix+(16<<CSF) >= objects[vb].x)
          {
            if (ypix <= objects[vb].y && ypix+(16<<CSF) >= objects[vb].y)
            {
              killboss(o);
            }
          }
       }
     }
     // clear the space at the top with black
     for(x=0;x<STONE_WIDTH;x++)
     {
       map_chgtile(x+xa,ya,BG_BLACK);
     }

     objects[o].ai.rope.stoneY++;
}

void killboss(int o)
{
int vb;
       vb = objects[o].ai.rope.vortboss;
       if (objects[vb].ai.vort.state != VORT_DYING &&\
           objects[vb].ai.vort.state != VORT_DEAD)
       {
           objects[vb].inhibitfall = 0;
           objects[vb].canbezapped = 0;
           objects[vb].ai.vort.animtimer = 0;
           objects[vb].ai.vort.frame = 0;
           objects[vb].ai.vort.palflashtimer = VORT_PALETTE_FLASH_TIME+1;
           objects[vb].ai.vort.palflashamt = 255;
           objects[vb].ai.vort.state = VORT_DYING;
           //pal_set(0, 255, objects[vb].ai.vort.palflashamt, objects[vb].ai.vort.palflashamt);
           g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
       }
}
