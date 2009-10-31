#include "CObjectAI.h"
#include "../../sdl/sound/CSound.h"
#include "../../keen.h"

// Teleporter on world map
// (animates the teleporter and moves the player)
// (ep1&3)

// rate at which the frame will change
#define TELEPORTER_ANIM_RATE_EP3    16
// number of times to change the frame before stopping
#define TELEPORTER_NUMFRAMES_EP3      16

#define TELEPORTER_ANIM_RATE_EP1    16
#define TELEPORTER_NUMFRAMES_EP1    20

void CObjectAI::teleporter_ai(CObject *p_object)
{
int mx,my;
int timeout;
int animrate, numframes;
   if (m_Episode!=3)
   {
     animrate = TELEPORTER_ANIM_RATE_EP1;
     numframes = TELEPORTER_NUMFRAMES_EP1;
   }
   else
   {
     animrate = TELEPORTER_ANIM_RATE_EP3;
     numframes = TELEPORTER_NUMFRAMES_EP3;
   }
   if (p_object->needinit)
   {  // first time initialization
     p_object->needinit = false;
     p_object->canbezapped = false;
     p_object->inhibitfall = true;

     //p_object->sprite = BlankSprite;
     p_object->sprite = 0;
     p_object->ai.teleport.animtimer = animrate + 1;
     p_object->ai.teleport.animframe = 0;
     p_object->ai.teleport.numframechanges = 0;
   }

   if (p_object->ai.teleport.animtimer > animrate)
   {
     mx = p_object->x >> CSF;
     my = p_object->y >> CSF;

     p_object->ai.teleport.animframe++;
     p_object->ai.teleport.animframe &= 3;
     p_object->ai.teleport.numframechanges++;
     p_object->ai.teleport.animtimer = 0;

     if (p_object->ai.teleport.numframechanges > numframes)
     { // animation is done
    	 mp_Map->setTile(mx, my, p_object->ai.teleport.idleframe);
       if (p_object->ai.teleport.direction==TELEPORTING_OUT)
       {  // teleporting out, go to new teleporter and new teleport in anim
         p_object->x = p_object->ai.teleport.destx;
         p_object->y = p_object->ai.teleport.desty;
         mp_Player[p_object->ai.teleport.whichplayer].x = p_object->ai.teleport.destx;
         mp_Player[p_object->ai.teleport.whichplayer].y = p_object->ai.teleport.desty;
         mp_Player[p_object->ai.teleport.whichplayer].pdir = DOWN;
         p_object->ai.teleport.direction = TELEPORTING_IN;
         p_object->needinit = true;
         g_pSound->playStereofromCoord(SOUND_TELEPORT, PLAY_NOW, p_object->scrx);
         // if we were told to snap the screen to the new position instead
         // of scrolling over to it, do that.
         if (p_object->ai.teleport.snap)
           mp_Map->gotoPos( p_object->ai.teleport.destx, p_object->ai.teleport.desty);

         if (p_object->ai.teleport.NoExitingTeleporter)
         {  // for the teleporter to exit the bonus area in ep1
            // which has no teleporter animation on the destination
            goto tport_done;
         }
       }
       else
       { // tport in and anim end: teleport complete so destroy tport object
tport_done: ;
		 mp_Player[p_object->ai.teleport.whichplayer].hideplayer = false;
         /*if (tobonuslevel)
         {
           player[p_object->ai.teleport.whichplayer].pdir = UP;
           levelcontrol.tobonuslevel = 0;
         }*/
         deleteObj(p_object);
       }
       return;
     }
     else
     { // teleport animation is not done. show the next frame
    	 mp_Map->setTile(mx, my, p_object->ai.teleport.baseframe + p_object->ai.teleport.animframe);
    	 mp_Map->redrawAt(mx, my);
     }
   }
   else
	 p_object->ai.teleport.animtimer++;
}
