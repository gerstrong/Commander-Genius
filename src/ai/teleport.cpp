#include "../sdl/sound/CSound.h"
#include "../keen.h"
#include "../include/game.h"
#include "../include/gamedo.h"

#include "../include/enemyai.h"

#include "../sdl/sound/CSound.h"


// Teleporter on world map
// (animates the teleporter and moves the player)
// (ep1&3)

// rate at which the frame will change
//#define TELEPORTER_ANIM_RATE_EP3    35                //original speed
#define TELEPORTER_ANIM_RATE_EP3    16
// number of times to change the frame before stopping
#define TELEPORTER_NUMFRAMES_EP3      16

#define TELEPORTER_ANIM_RATE_EP1    16
#define TELEPORTER_NUMFRAMES_EP1    20

// Reference to ../game.cpp
void delete_object(int o);

void teleporter_ai(int o, stLevelControl levelcontrol)
{
int mx,my;
int timeout;
int animrate, numframes;
   if (levelcontrol.episode!=3)
   {
     animrate = TELEPORTER_ANIM_RATE_EP1;
     numframes = TELEPORTER_NUMFRAMES_EP1;
   }
   else
   {
     animrate = TELEPORTER_ANIM_RATE_EP3;
     numframes = TELEPORTER_NUMFRAMES_EP3;
   }

   if (objects[o].needinit)
   {  // first time initialization
     objects[o].needinit = 0;
     objects[o].canbezapped = 0;
     objects[o].inhibitfall = 1;

     objects[o].sprite = BlankSprite;
     objects[o].ai.teleport.animtimer = animrate + 1;
     objects[o].ai.teleport.animframe = 0;
     objects[o].ai.teleport.numframechanges = 0;
   }

   if (objects[o].ai.teleport.animtimer > animrate)
   {
     mx = objects[o].x >> CSF >> 4;
     my = objects[o].y >> CSF >> 4;

     objects[o].ai.teleport.animframe++;
     objects[o].ai.teleport.animframe &= 3;
     objects[o].ai.teleport.numframechanges++;
     objects[o].ai.teleport.animtimer = 0;

     if (objects[o].ai.teleport.numframechanges > numframes)
     { // animation is done
       map_chgtile(mx, my, objects[o].ai.teleport.idleframe);
       if (objects[o].ai.teleport.direction==TELEPORTING_OUT)
       {  // teleporting out, go to new teleporter and new teleport in anim
         objects[o].x = objects[o].ai.teleport.destx;
         objects[o].y = objects[o].ai.teleport.desty;
         player[objects[o].ai.teleport.whichplayer].x = objects[o].ai.teleport.destx;
         player[objects[o].ai.teleport.whichplayer].y = objects[o].ai.teleport.desty;
         player[objects[o].ai.teleport.whichplayer].pdir = DOWN;
         objects[o].ai.teleport.direction = TELEPORTING_IN;
         objects[o].needinit = 1;
         g_pSound->playStereofromCoord(SOUND_TELEPORT, PLAY_NOW, objects[o].scrx);
         // if we were told to snap the screen to the new position instead
         // of scrolling over to it, do that.
         if (objects[o].ai.teleport.snap)
         {
           timeout = 0;
           while(gamedo_ScrollTriggers(primaryplayer))
           {
             if (++timeout>10000)
             {
               crashflag = 1;
               why_term_ptr = "teleport_ai(): timed out snapping screen to new position.";
               return;
             }
           }
           // white-out teleport flash
           fade.mode = FADE_GO;
           fade.dir = FADE_IN;
           fade.curamt = PAL_FADE_WHITEOUT;
           fade.fadetimer = 0;
           fade.rate = FADE_FAST;
         }
         if (objects[o].ai.teleport.NoExitingTeleporter)
         {  // for the teleporter to exit the bonus area in ep1
            // which has no teleporter animation on the destination
            goto tport_done;
         }
       }
       else
       { // tport in and anim end: teleport complete so destroy tport object
tport_done: ;
         player[objects[o].ai.teleport.whichplayer].hideplayer = 0;
         if (levelcontrol.tobonuslevel)
         {
           player[objects[o].ai.teleport.whichplayer].pdir = UP;
           levelcontrol.tobonuslevel = 0;
         }
         delete_object(o);
       }
       return;
     }
     else
     { // teleport animation is not done. show the next frame
       map_chgtile(mx, my, objects[o].ai.teleport.baseframe + objects[o].ai.teleport.animframe);
     }
   }
   else
   {
     objects[o].ai.teleport.animtimer++;
   }

}
