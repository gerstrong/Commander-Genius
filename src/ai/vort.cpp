#include "../sdl/sound/CSound.h"
#include "../sdl/CVideoDriver.h"
#include "../keen.h"
#include "vort.h"
#include "../include/game.h"

#include "../include/enemyai.h"



// Vorticon (all episodes, albeit the behavior changes slightly
// depending on levelcontrol.episode).

// Reference to ../misc.cpp
unsigned int rnd(void);

void vort_initiatejump(int o);

void vort_ai(int o, stLevelControl *p_levelcontrol)
{
/*int bonk,kill;
   if (objects[o].needinit)
   {  // first time initilization
     objects[o].ai.vort.frame = 0;
     objects[o].ai.vort.animtimer = 0;
     objects[o].ai.vort.state = VORT_LOOK;
     objects[o].ai.vort.timer = 0;
     objects[o].ai.vort.dist_traveled = VORT_TRAPPED_DIST+1;
     objects[o].canbezapped = 1;
     objects[o].needinit = 0;

     // give him some health points, depening on episode and level
     if(p_levelcontrol->episode == 1)
     {
    	 if(p_levelcontrol->curlevel == 16) // He is the vorticon commander and has much more HP
    		 objects[o].ai.vort.hp = 105;
    	 else
    		 objects[o].ai.vort.hp = 4;
     }
     else
    	 objects[o].ai.vort.hp = (p_levelcontrol->hardmode) ? 4 : 1;
		 // In EP2 and 3 the Vorticons are much weaker, when no hardmode selected

     // copy in animation frame indexes for the current ep
     if (p_levelcontrol->episode==1)
     {
       objects[o].ai.vort.WalkLeftFrame = VORT1_WALK_LEFT_FRAME;
       objects[o].ai.vort.WalkRightFrame = VORT1_WALK_RIGHT_FRAME;
       objects[o].ai.vort.LookFrame = VORT1_LOOK_FRAME;
       objects[o].ai.vort.JumpRightFrame = VORT1_JUMP_RIGHT_FRAME;
       objects[o].ai.vort.JumpLeftFrame = VORT1_JUMP_LEFT_FRAME;
       objects[o].ai.vort.DyingFrame = VORT1_DYING_FRAME;
     }
     else if (p_levelcontrol->episode==2)
     {
       objects[o].ai.vort.WalkLeftFrame = VORT2_WALK_LEFT_FRAME;
       objects[o].ai.vort.WalkRightFrame = VORT2_WALK_RIGHT_FRAME;
       objects[o].ai.vort.LookFrame = VORT2_LOOK_FRAME;
       objects[o].ai.vort.JumpRightFrame = VORT2_JUMP_RIGHT_FRAME;
       objects[o].ai.vort.JumpLeftFrame = VORT2_JUMP_LEFT_FRAME;
       objects[o].ai.vort.DyingFrame = VORT2_DYING_FRAME;
       objects[o].ai.vort.DeadFrame = VORT2_DEAD_FRAME;
     }
     else if (p_levelcontrol->episode==3)
     {
       objects[o].ai.vort.WalkLeftFrame = VORT3_WALK_LEFT_FRAME;
       objects[o].ai.vort.WalkRightFrame = VORT3_WALK_RIGHT_FRAME;
       objects[o].ai.vort.LookFrame = VORT3_LOOK_FRAME;
       objects[o].ai.vort.JumpRightFrame = VORT3_JUMP_RIGHT_FRAME;
       objects[o].ai.vort.JumpLeftFrame = VORT3_JUMP_LEFT_FRAME;
       objects[o].ai.vort.DyingFrame = VORT3_DYING_FRAME;
       objects[o].ai.vort.DeadFrame = VORT3_DEAD_FRAME;
     }
   }
   if (objects[o].ai.vort.state==VORT_DEAD)   return;

   if (objects[o].canbezapped)
   {
     kill = 0;
     // if we touch a glowcell, we die!

     if ( objects[o].zapped >= objects[o].ai.vort.hp ) kill = 1;
     else if (p_levelcontrol->episode==2 && getmaptileat((objects[o].x>>CSF)+12, (objects[o].y>>CSF)+16)==TILE_GLOWCELL)
       kill = 1;

     if (kill)
     {
       objects[o].inhibitfall = 0;
       objects[o].canbezapped = 0;
       objects[o].ai.vort.animtimer = 0;
       objects[o].ai.vort.frame = 0;
       objects[o].ai.vort.palflashtimer = VORT_PALETTE_FLASH_TIME + 1;
       objects[o].ai.vort.palflashamt = 255;
       if (p_levelcontrol->episode == 1)
       {
           g_pGfxEngine->Palette.setFadeColour(SDL_MapRGB(g_pVideoDriver->FXSurface->format,
    										   255,255,255));
           g_pGfxEngine->Palette.fadeto(200, FADE_SPEED_FAST);
           objects[o].ai.vort.state = VORT_DYING;
       }
       else
    	   objects[o].ai.vort.state = VORT2_DYING;

       g_pSound->playStereofromCoord(SOUND_VORT_DIE, PLAY_NOW, objects[o].scrx);
     }
   }
   // deadly to the touch
   if (objects[o].touchPlayer && objects[o].canbezapped)
   {
     killplayer(objects[o].touchedBy);
   }

vort_reprocess: ;
   switch(objects[o].ai.vort.state)
   {
    case VORT_JUMP:
       if (objects[o].ai.vort.movedir == RIGHT)
       {
         if (!objects[o].blockedr) objects[o].x += VORT_WALK_SPEED;
       }
       else
       {
         if (!objects[o].blockedl) objects[o].x -= VORT_WALK_SPEED;
       }

       if (objects[o].ai.vort.inertiay>0 && objects[o].blockedd)
       {  // The Vorticon Has Landed!
          objects[o].inhibitfall = 0;
          objects[o].ai.vort.state = VORT_LOOK;
          goto vort_reprocess;
       }
       // check if the vorticon has bonked into a ceiling, if so,
       // immediately terminate the jump
       bonk = 0;

       if (TileProperty[getmaptileat((objects[o].x>>CSF)+1, (objects[o].y>>CSF))][BDOWN]) bonk = 1;
       else if (TileProperty[getmaptileat((objects[o].x>>CSF)+16, (objects[o].y>>CSF))][BDOWN]) bonk = 1;
       else if (TileProperty[getmaptileat((objects[o].x>>CSF)+23, (objects[o].y>>CSF))][BDOWN]) bonk = 1;


       if (bonk && objects[o].ai.vort.inertiay < 0)
       {
          objects[o].ai.vort.inertiay = 0;
       }

       // apply Y inertia
       objects[o].y += objects[o].ai.vort.inertiay;

       if (objects[o].ai.vort.timer > VORT_JUMP_FRICTION)
       { // slowly decrease upgoing rate
          if (objects[o].ai.vort.inertiay<VORT_MAX_FALL_SPEED)
          {
            objects[o].ai.vort.inertiay++;
          }
          objects[o].ai.vort.timer = 0;
       }
       else objects[o].ai.vort.timer++;
    break;
    case VORT_LOOK:
       objects[o].sprite = objects[o].ai.vort.LookFrame + objects[o].ai.vort.frame;

       if (objects[o].ai.vort.animtimer > VORT_LOOK_ANIM_TIME)
       {
         if (objects[o].ai.vort.frame>0)
         {
           if (objects[o].blockedl)
             { objects[o].ai.vort.movedir = RIGHT; }
           else if (objects[o].blockedr)
             { objects[o].ai.vort.movedir = LEFT; }
           else
           { // not blocked on either side, head towards player
        	 //  player[primaryplayer].useObject!!!

       		   if ( player[primaryplayer].x < objects[o].x)
       		   { objects[o].ai.vort.movedir = LEFT; }
       		   else
			   { objects[o].ai.vort.movedir = RIGHT; }
           }
           objects[o].ai.vort.timer = 0;
           objects[o].ai.vort.frame = 0;
           objects[o].ai.vort.state = VORT_WALK;
         } else objects[o].ai.vort.frame++;
         objects[o].ai.vort.animtimer = 0;
       } else objects[o].ai.vort.animtimer++;
       break;
     case VORT_WALK:
       objects[o].ai.vort.dist_traveled++;

       if (rnd()%VORT_JUMP_PROB == (VORT_JUMP_PROB/2))
       {  // let's jump.
         if (!p_levelcontrol->dark && !objects[o].blockedu)
         {
           vort_initiatejump(o);
           goto vort_reprocess;
         }
       }
       if (objects[o].ai.vort.movedir==LEFT)
       {  // move left
         objects[o].sprite = objects[o].ai.vort.WalkLeftFrame + objects[o].ai.vort.frame;

         if (!objects[o].blockedl)
         {
           objects[o].x -= VORT_WALK_SPEED;
         }
         else
         {
           objects[o].ai.vort.frame = 0;
           objects[o].ai.vort.animtimer = 0;
           objects[o].ai.vort.state = VORT_LOOK;

           // if we only traveled a tiny amount before hitting a wall, we've
           // probably fallen into a small narrow area, and we need to try
           // to jump out of it
           if (objects[o].ai.vort.dist_traveled < VORT_TRAPPED_DIST && !p_levelcontrol->dark && objects[o].blockedd && !objects[o].blockedu)
           {
             vort_initiatejump(o);
             if (rnd()&1)
             {
               objects[o].ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
             }
             else
             {
               objects[o].ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
             }
             goto vort_reprocess;
           }
           else objects[o].ai.vort.dist_traveled = 0;
         }
       }
       else
       {  // move right
         objects[o].sprite = objects[o].ai.vort.WalkRightFrame + objects[o].ai.vort.frame;

         if (!objects[o].blockedr)
         {
           objects[o].x += VORT_WALK_SPEED;
         }
         else
         {
           objects[o].ai.vort.frame = 0;
           objects[o].ai.vort.animtimer = 0;
           objects[o].ai.vort.state = VORT_LOOK;

           if (objects[o].ai.vort.dist_traveled < VORT_TRAPPED_DIST && !p_levelcontrol->dark && objects[o].blockedd && !objects[o].blockedu)
           {
             vort_initiatejump(o);
             if (rand()&1)
             {
               objects[o].ai.vort.inertiay = -VORT_MAX_JUMP_HEIGHT;
             }
             else
             {
               objects[o].ai.vort.inertiay = -VORT_MIN_JUMP_HEIGHT;
             }
             goto vort_reprocess;
           }
           else objects[o].ai.vort.dist_traveled = 0;
         }
       }
       // walk animation
       if (objects[o].ai.vort.animtimer > VORT_WALK_ANIM_TIME)
       {
         if (objects[o].ai.vort.frame>=3) objects[o].ai.vort.frame=0;
                                     else objects[o].ai.vort.frame++;
         objects[o].ai.vort.animtimer = 0;
       } else objects[o].ai.vort.animtimer++;
    break;
    case VORT_DYING:
    objects[o].sprite = objects[o].ai.vort.DyingFrame + objects[o].ai.vort.frame;

       if (objects[o].ai.vort.animtimer > VORT_DIE_ANIM_TIME)
       {
   		   objects[o].ai.vort.frame++;
   		   if (objects[o].ai.vort.frame>=6)
   		   {
   			objects[o].ai.vort.frame = 5;
             g_pGfxEngine->Palette.fadeto(0, FADE_SPEED_VERY_SLOW);
             if(!g_pGfxEngine->Palette.in_progress())
            	 objects[o].ai.vort.state = VORT_DEAD;
   		   }

         objects[o].ai.vort.animtimer = 0;
       } else objects[o].ai.vort.animtimer++;
    break;
    case VORT2_DYING:
       objects[o].sprite = objects[o].ai.vort.DyingFrame;
       if (objects[o].ai.vort.animtimer > VORT2_DIE_ANIM_TIME)
       {
         objects[o].sprite = objects[o].ai.vort.DeadFrame;
         objects[o].ai.vort.state = VORT_DEAD;
       }
       else
       {
         objects[o].ai.vort.animtimer++;
       }
    break;
    default: break;
   }
}

void vort_initiatejump(int o)
{
       // must be standing on floor to jump
       if (!objects[o].blockedd) return;

           objects[o].ai.vort.frame = 0;
           objects[o].ai.vort.animtimer = 0;
           objects[o].ai.vort.inertiay = \
             -((rnd()%(VORT_MAX_JUMP_HEIGHT-VORT_MIN_JUMP_HEIGHT))+VORT_MIN_JUMP_HEIGHT);

           if (objects[o].ai.vort.movedir==RIGHT)
              objects[o].sprite = objects[o].ai.vort.JumpRightFrame;
           else
              objects[o].sprite = objects[o].ai.vort.JumpLeftFrame;

           objects[o].inhibitfall = 1;
           objects[o].ai.vort.state = VORT_JUMP;*/
}
