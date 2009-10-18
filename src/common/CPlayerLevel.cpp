/*
 * CPlayerLevel.cpp
 *
 *  Created on: 07.10.2009
 *      Author: gerstrong
 */

#include "CPlayer.h"

#include "../keen.h"
#include "../sdl/sound/CSound.h"
#include "../sdl/CInput.h"
#include "../graphics/CGfxEngine.h"

#define PDIEFRAME             22

////
// Process the stuff of the player when playing in a normal level
void CPlayer::processInLevel()
{
    StatusBox();

    if (pdie) dieanim();
	else
	{
	  inhibitwalking = false;
	  inhibitfall = false;

	  ProcessInput();

	  setDir();

	  getgoodies();

	  ankh(); // Normally can happen in Episode 3

	  raygun();

	  keencicle();

	  if(!pjumping && !pfalling)
	  {
		  Walking();
		  WalkingAnimation();
	  }

	  playpushed();
	  InertiaAndFriction_X();

	  TogglePogo_and_Switches();
	  JumpAndPogo();
	}
    SelectFrame();
}

void CPlayer::walkbehindexitdoor()
{
/*int xb, diff, width;

    // don't draw keen as he walks through the door (past exitXpos)
    // X pixel position of right side of player
    xb = (x >> CSF) + PLAYERSPRITE_WIDTH;
    diff = (xb - pCKP->Control.levelcontrol.exitXpos);        // dist between keen and door
    if (diff >= 0)                             // past exitXpos?
    {
       width = (PLAYERSPRITE_WIDTH - diff);    // get new width of sprite
       if (width < 0) width = 0;               // don't set to negative

       // set new width of all player walk frames
       g_pGfxEngine->Sprite[playerbaseframe+0]->setWidth(width);
       g_pGfxEngine->Sprite[playerbaseframe+1]->setWidth(width);
       g_pGfxEngine->Sprite[playerbaseframe+2]->setWidth(width);
       g_pGfxEngine->Sprite[playerbaseframe+3]->setWidth(width);
    }*/
}

void CPlayer::dieanim() // Bad word for that. It's the entire die code
{
   /*if (!pdie) return;                // should never happen...
   if (pdie==PDIE_DEAD) return;      // if true animation is over
   if (pdie==PDIE_FELLOFFMAP)
   {
     // wait for falling sound to complete, then kill the player
     if (!g_pSound->isPlaying(SOUND_KEEN_FALL))
     {
        pdie = false;
        killplayer(cp);
     }
     else return;
   }

   // peridocally toggle dying animation frame
   if (pdietimer > DIE_ANIM_RATE)
   {
     pdieframe = 1 - pdieframe;
     pdietimer = 0;
   }
   else pdietimer++;

   // is it time to start flying off the screen?
   if (!pdietillfly)
   {  // time to fly off the screen
     if (((y>>CSF)+96 > scroll_y) && (y>(16<<CSF)))
     {  // player has not reached top of screen
        // make player fly up
        y += PDIE_RISE_SPEED;
        if (x > (4<<CSF))
        {
          x += pdie_xvect;
        }
     }
     else
     {  // reached top of screen. he's done.
       pdie = PDIE_DEAD;
       if (inventory.lives<0)
       {
    	   p_levelcontrol->gameovermode = true;
    	   g_pSound->playSound(SOUND_GAME_OVER, PLAY_NOW);
    	   CBitmap *bm_gameover = g_pGfxEngine->getBitmap("GAMEOVER");
    	   // figure out where to center the gameover bitmap and draw it
		   int x = (g_pVideoDriver->getGameResRect().w/2)-(bm_gameover->getWidth()/2);
		   int y = (g_pVideoDriver->getGameResRect().h/2)-(bm_gameover->getHeight()/2);
		   bm_gameover->draw(g_pVideoDriver->BlitSurface, x, y);

       }
       else
       {
         endlevel(0,p_levelcontrol);
         p_levelcontrol->chglevelto = WM_MAP_NUM;
         p_levelcontrol->command = LVLC_CHANGE_LEVEL;
       }
     }
   }
   else
   {  // not yet time to fly off screen, decrement timer
     pdietillfly--;
   }  // end "time to fly"
   */
}

void CPlayer::keencicle()
{
   // keencicle code (when keen gets hit by an ice chunk)
   if(pfrozentime)
   {
     if (pfrozentime > PFROZEN_THAW)
     {
        if (pfrozenanimtimer > PFROZENANIMTIME)
        {
          if (pfrozenframe) pfrozenframe=0; else pfrozenframe=1;
          pfrozenanimtimer = 0;
        }
        else pfrozenanimtimer++;
     }
     else
     { // thawing out, show the thaw frame
        if (m_episode==3)
          pfrozenframe = 2;
        else
          pfrozenframe = 3;
     }

     pfrozentime--;
     inhibitwalking = true;
   }
}

// if player not sliding and not jumping, allow
// them to change their direction. if jumping,
// we can change direction but it will not be shown
// in the frame.
void CPlayer::setDir()
{
   if (pfrozentime) return;
   // can't change direction on ice,
   // UNLESS we're stuck up against a wall
   if (psliding)
   {
	 bool stuck = false;
	 if (pshowdir == LEFT && blockedl) stuck = true;
	 if (pshowdir == RIGHT && blockedr) stuck = true;
	 if (stuck)
	 {
	   // jumped off an ice block into a wall?
	   if (pjumping || pfalling)
	   {
		 psliding = 0;
	   }
	 }
	 else
	 {
	   // since we're not stuck up against a wall, we can't change direction
	   return;
	 }
   }

   if (!pjumping && !pfiring)
   {
	 if (playcontrol[PA_X] < 0) { pdir = pshowdir = LEFT; }
	 if (playcontrol[PA_X] > 0) { pdir = pshowdir = RIGHT; }
   }
   else
   {
	 if (playcontrol[PA_X] < 0) { pdir = pshowdir = LEFT;  }
	 if (playcontrol[PA_X] > 0) { pdir = pshowdir = RIGHT;  }
   }
}

// let's have keen be able to pick up goodies
void CPlayer::getgoodies()
{
   /*if ((TileProperty[getmaptileat((x>>CSF)+9, (y>>CSF)+1)][BEHAVIOR] > 0) &&
		   ( TileProperty[getmaptileat((x>>CSF)+9, (y>>CSF)+1)][BEHAVIOR] < 31 ) )
      { keen_get_goodie((x>>CSF)+9, (y>>CSF)+1, cp, pCKP); return; }

   else if ((TileProperty[getmaptileat((x>>CSF)+4, (y>>CSF)+8)][BEHAVIOR] > 0) &&
		   ( TileProperty[getmaptileat((x>>CSF)+4, (y>>CSF)+8)][BEHAVIOR] < 31 ) )
      { keen_get_goodie((x>>CSF)+4, (y>>CSF)+8, cp, pCKP); return; }

   else if ((TileProperty[getmaptileat((x>>CSF)+9, (y>>CSF)+16)][BEHAVIOR] > 0) &&
		   ( TileProperty[getmaptileat((x>>CSF)+9, (y>>CSF)+16)][BEHAVIOR] < 31 ) )
      { keen_get_goodie((x>>CSF)+9, (y>>CSF)+16, cp, pCKP); return; }

   else if ((TileProperty[getmaptileat((x>>CSF)+4, (y>>CSF)+23)][BEHAVIOR] > 0) &&
		   ( TileProperty[getmaptileat((x>>CSF)+4, (y>>CSF)+23)][BEHAVIOR] < 31 ) )
      { keen_get_goodie((x>>CSF)+4, (y>>CSF)+23, cp, pCKP); return; }*/
}

// handle playpushed_x: for yorps/scrubs/etc pushing keen
void CPlayer::playpushed()
{
    /*//if (mp_option[OPT_CHEATS].value && g_pInput->getHoldedKey(KTAB)) return;

    // if we're being pushed...
    if (playpushed_x)
    {
      // do friction on push force...
      if (playpushed_decreasetimer>PLAYPUSH_DECREASERATE)
      {
        // push playpushed_x towards zero
        if (playpushed_x < 0)
        {
          playpushed_x++;
        }
        else
        {
          playpushed_x--;
        }
        playpushed_decreasetimer = 0;
      }
      else playpushed_decreasetimer++;

      // if we run up against a wall all push inertia stops
      if (playpushed_x > 0 && blockedr) playpushed_x = 0;
      if (playpushed_x < 0 && blockedl) playpushed_x = 0;
    }*/
}

// called when a switch is flipped. mx,my is the pixel coords of the switch,
// relative to the upper-left corner of the map.
/*void CPlayer::ExtendingPlatformSwitch(int x, int y, stLevelControl *p_levelcontrol)
{
uint ppos;
int platx, platy;
signed char pxoff, pyoff;
int mapx, mapy;
int o;

	// convert pixel coords to tile coords
	mapx = (x >> TILE_S);
	mapy = (y >> TILE_S);

	// figure out where the platform is supposed to extend at
	// (this is coded in the object layer...
	// high byte is the Y offset and the low byte is the X offset,
	// and it's relative to the position of the switch.)
	ppos = getlevelat(x, y);

	if (!ppos || !p_levelcontrol->PlatExtending)
	{
		// flip switch
		g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, mapx);
		if (getmaptileat(x, y)==TILE_SWITCH_DOWN)
			map_chgtile(mapx, mapy, TILE_SWITCH_UP);
		else
			map_chgtile(mapx, mapy, TILE_SWITCH_DOWN);
	}

	// if zero it means he hit the switch on a tantalus ray!
	if (!ppos)
	{
		p_levelcontrol->success = 0;
		p_levelcontrol->command = LVLC_TANTALUS_RAY;
		return;
	}
	else
	{
		// it's a moving platform switch--don't allow player to hit it again while
		// the plat is still moving as this will glitch
		if (p_levelcontrol->PlatExtending) return;
		p_levelcontrol->PlatExtending = 1;
	}

	pxoff = (ppos & 0x00ff);
	pyoff = (ppos & 0xff00) >> 8;
	platx = mapx + pxoff;
	platy = mapy + pyoff;

	// spawn a "sector effector" to extend/retract the platform
	o = spawn_object(mapx<<TILE_S<<CSF,mapy<<TILE_S<<CSF,OBJ_SECTOREFFECTOR);
	objects[o].ai.se.type = SE_EXTEND_PLATFORM;
	objects[o].ai.se.platx = platx;
	objects[o].ai.se.platy = platy;
}*/

// allow Keen to toggle the pogo stick and hit switches
void CPlayer::TogglePogo_and_Switches()
{
/*int i;
int mx, my, t;
CSprite *standsprite = g_pGfxEngine->Sprite[PSTANDFRAME];

	// detect if KPOGO key only pressed
	if (playcontrol[PA_POGO] && !lastplaycontrol[PA_POGO] && !pfrozentime)
	{
		// if we are standing near a switch hit the switch instead
		mx = (x>>CSF)+(standsprite->getWidth()/2);

		for(i=standsprite->getHeight()-1;i>=0;i-=8)
		{
			my = (y>>CSF)+i;

			t = getmaptileat(mx, my);

			// check for extending-platform switch
			if (t==TILE_SWITCH_UP || t==TILE_SWITCH_DOWN )
			{
				ExtendingPlatformSwitch(mx, my, p_levelcontrol);
				if (!ppogostick) return;
			}
			else if (t==TILE_LIGHTSWITCH)
			{ // lightswitch
				   p_levelcontrol->dark ^= 1;
				   g_pGfxEngine->Palette.setdark(p_levelcontrol->dark);
				   g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, objects[useObject].scrx);
				if (!ppogostick) return;
			}
		}

		// toggle pogo stick
		if (inventory.HasPogo)
		{
			ppogostick ^= 1;
		}
	}*/
}

void CPlayer::JumpAndPogo()
{
   // handle the JUMP key, both for normal jumps and (high) pogo jumps
   if (!pjumping && !pfalling && !pfiring)
   {
	 pboost_x = 0;
     // give em the chance to jump
     if (playcontrol[PA_JUMP] && !ppogostick && !pfrozentime)
     {
       pjumping = PPREPAREJUMP;
       pjumpframe = PPREPAREJUMPFRAME;
       pjumpanimtimer = 0;
       pwalking = 0;
     }
     else if (ppogostick)
     {
       pjumping = PPREPAREPOGO;
       pjumpanimtimer = 0;
       pwalking = 0;
     }
   }

    switch(pjumping)
    {
      case PPREPAREPOGO:
          if (pjumpanimtimer>PPOGO_PREPARE_TIME)
          {
             // continously bounce while pogo stick is out
        	  g_pSound->playStereofromCoord(SOUND_KEEN_JUMP, PLAY_NOW, mp_object->scrx);

        	  // jump high if JUMP key down, else bounce low
        	  if (playcontrol[PA_JUMP])
        	  {
				   if (!mp_option[OPT_SUPERPOGO].value)
				   {  // normal high pogo jump
					  if(playcontrol[PA_JUMP] > 12)
						  pjumpupspeed = ((PPOGOUP_SPEED-PJUMPUP_SPEED)*playcontrol[PA_JUMP]) / 50 + PJUMPUP_SPEED;
					  else
						  pjumpupspeed = (PPOGOUP_SPEED*11) / 10; // Impossible Pogo Trick
					  pjumptime = PJUMP_NORMALTIME_POGO_LONG;
					  pjumpupdecreaserate = PJUMP_UPDECREASERATE_POGO_LONG;
				   }
				   else
				   {
					  pjumpupspeed = PPOGOUP_SPEED_SUPER;
					  pjumptime = PJUMP_NORMALTIME_POGO_LONG_SUPER;
					  pjumpupdecreaserate = PJUMP_UPDECREASERATE_POGO_LONG_SUPER;
				   }
        	  }
        	  else
        	  {
        		  if(ppogostick)
        		  {
        			  pjumpupspeed = PJUMPUP_SPEED;
        			  pjumptime = PJUMP_NORMALTIME_POGO_SHORT;
        			  pjumpupdecreaserate = PJUMP_UPDECREASERATE_POGO_SHORT;
        		  }
        	  }
        	  pjumpframe = PJUMP_PREPARE_LAST_FRAME;
        	  pjumping = PPOGOING;
        	  pjumpupspeed_decreasetimer = 0;
        	  pjustjumped = 1;

          } else pjumpanimtimer++;
          break;
      case PPREPAREJUMP:
			 widejump = true;

   			 if(psliding)
   			 {
   				 if(pdir == LEFT)
   					 chargedjump-=2;
   				 else if(pdir == RIGHT)
   					chargedjump+=2;
   			 }
   			 else
   			 {
   	   			 if(g_pInput->getHoldedCommand(IC_LEFT))
   	   				 chargedjump-=2;
   	   			 else if(g_pInput->getHoldedCommand(IC_RIGHT))
   	   				 chargedjump+=2;
   			 }

             pinertia_x = 0;     // prevent moving while preparing to jump
             if (pjumpanimtimer > PJUMP_PREPARE_ANIM_RATE)
             {
                  if (pjumpframe == PJUMP_PREPARE_LAST_FRAME || !playcontrol[PA_JUMP])
                  {  	// time to start the jump
						// select a jump depending on how long keen was preparing
                       pjumpupspeed = PJUMPUP_SPEED;
                       switch(pjumpframe)
                       {
                       case PPREPAREJUMPFRAME:
                            pjumptime = PJUMP_NORMALTIME_6;
                            pjumpupdecreaserate = PJUMP_UPDECREASERATE_6;
                            pjumpupspeed = 1;
                            chargedjump = chargedjump >> 5;
                            break;
                       case PPREPAREJUMPFRAME+1:
                            pjumptime = PJUMP_NORMALTIME_5;
                            pjumpupdecreaserate = PJUMP_UPDECREASERATE_5;
                            pjumpupspeed = 2;
                            chargedjump = chargedjump >> 4;
                            break;
                       case PPREPAREJUMPFRAME+2:
                            pjumptime = PJUMP_NORMALTIME_4;
                            pjumpupdecreaserate = PJUMP_UPDECREASERATE_4;
                            pjumpupspeed = 4;
                            chargedjump = chargedjump >> 3;
                            break;
                       case PPREPAREJUMPFRAME+3:
                            pjumptime = PJUMP_NORMALTIME_3;
                            pjumpupdecreaserate = PJUMP_UPDECREASERATE_3;
                            pjumpupspeed = 8;
                            chargedjump = chargedjump >> 2;
                            break;
                       case PPREPAREJUMPFRAME+4:
                            pjumptime = PJUMP_NORMALTIME_2;
                            pjumpupdecreaserate = PJUMP_UPDECREASERATE_2;
                            pjumpupspeed = 16;
                            chargedjump = chargedjump >> 1;
                            break;
                       default:
                            pjumptime = PJUMP_NORMALTIME_1;
                            pjumpupdecreaserate = PJUMP_UPDECREASERATE_1;
                            break;
                       }

                    pjumpframe = PJUMP_PREPARE_LAST_FRAME;

                    g_pSound->playStereofromCoord(SOUND_KEEN_JUMP, PLAY_NOW, mp_object->scrx);
                    pjumping = PJUMPUP;
                    pjumpupspeed_decreasetimer = 0;
                    pjustjumped = 1;
                    pjumpfloattimer = 0;

                    // make so if we're jumping left or right
                    // the walk code will start at full speed
                    pwalking = 1;
                    pwalkanimtimer = 0;
                    pwalkframe = 1;
                    if ( g_pGfxEngine->Tilemap->mp_tiles[psupportingtile].behaviour == 3)
                    { // on ice, always jump direction facing
                      if (pshowdir==LEFT)
                        {  pdir=LEFT; }
                      else
                        {  pdir=RIGHT; }
                    }
                    else
                      pjumpdir = UP;

                    if (playcontrol[PA_X] < 0)
                    	pinertia_x--;
                    if (playcontrol[PA_X] > 0)
                    	pinertia_x++;

                    pwalkincreasetimer = 0;
                  }
                  else
                  {
                    pjumpframe++;
                  }
                  pjumpanimtimer=0;
             } else pjumpanimtimer++;
             break;
        case PJUMPUP:
        	pinertia_x += chargedjump;
        	chargedjump = 0;
        case PPOGOING:
        // check for hitting a ceiling
         if (blockedu)   // did we bonk something?
         {  // immediatly abort the jump
            pjumping = PNOJUMP;
            g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->scrx);
         }

         // do the jump
         if (!pjumptime)
         {
           if (pjumpupspeed_decreasetimer>pjumpupdecreaserate)
           {
       		   if (!pjumpupspeed) pjumping = PNOJUMP;
       		   else pjumpupspeed--;

       		   pjumpupspeed_decreasetimer=0;
           } else pjumpupspeed_decreasetimer++;
         }
         else pjumptime--;

         goto_y -= pjumpupspeed;
         break;
    }

    // If we are in Godmode, use the Pogo, and pressing the jump button, make the player fly
    if( godmode && ppogostick &&
    		g_pInput->getHoldedCommand(0, IC_JUMP) && !blockedu )
    	goto_y -= PPOGOUP_SPEED;
}

// wouldn't it be cool if keen had a raygun, and he could shoot things?
// oh wait, he does, and here's the code for it.
void CPlayer::raygun()
{
/*int o;
int canRefire;

	stOption *p_option;
	p_option = pCKP->Option;

   if (pfireframetimer) pfireframetimer--;

   // FIRE button down, and not keencicled?
   if (playcontrol[PA_FIRE] && !pfrozentime)
   {
     inhibitwalking = 1;    // prevent moving
     pfiring = 1;           // flag that we're firing
     ppogostick = false;        // put away pogo stick if out

     if (!lastplaycontrol[PA_FIRE] || mp_option[OPT_FULLYAUTOMATIC].value)
     { // fire is newly pressed

       // limit how quickly shots can be fired
       if ( mp_option[OPT_FULLYAUTOMATIC].value )
       {
         if (pfireframetimer < PFIRE_LIMIT_SHOT_FREQ_FA)
         {
           canRefire = 1;
         }
         else canRefire = 0;
       }

       else
       {
         if (pfireframetimer < PFIRE_LIMIT_SHOT_FREQ)
         {
           canRefire = 1;
         }
         else canRefire = 0;
       }

       if (canRefire)
       {
          // show raygun for a minimum time even if FIRE is immediatly released
          pfireframetimer = PFIRE_SHOWFRAME_TIME;

          // try to fire off a blast
          if (inventory.charges)
          {  // we have enough charges

             inventory.charges--;
             pshowdir = pdir;

             g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, objects[useObject].scrx);

             if (pdir==RIGHT)
             {  // fire a blast to the right
                o = spawn_object(x+((g_pGfxEngine->Sprite[0]->getWidth()-4)<<CSF), y+(9<<CSF), OBJ_RAY);
                objects[o].ai.ray.direction = RIGHT;
             }
             else
             {  // fire a blast to the left
                o = spawn_object(x-(12<<CSF), y+(9<<CSF), OBJ_RAY);
                objects[o].ai.ray.direction = LEFT;
             }
              // if '-nopk' argument set don't kill other players
              if (p_option[OPT_ALLOWPKING].value)
              {
                objects[o].ai.ray.dontHitEnable = 0;
              }
              else
              {
                objects[o].ai.ray.dontHitEnable = 1;
                objects[o].ai.ray.dontHit = OBJ_PLAYER;
              }
          }
          else
          { // oh shit, out of bullets
            // click!
        	  g_pSound->playStereofromCoord(SOUND_GUN_CLICK, PLAY_NOW, objects[useObject].scrx);

          }  // end "do we have charges?"

       } // end "limit how quickly shots can be fired"

     } // end "fire is newly pressed"
   } // end "fire button down and not keencicled"
   else
   { // FIRE button is NOT down
      // put away ray gun after it's shown for the minimum period of time
      if (!pfireframetimer)
      {  // ray gun shown for minimum time
        pfiring = 0;
      }
      else
      {  // minimum time not expired
        pfiring = 1;
        inhibitwalking = 1;
      }
   }
*/
}

// select the appropriate player frame based on what he's doing
void CPlayer::SelectFrame()
{
    playframe = 0;      // basic standing

    // select the frame assuming he's pointing right. ep1 does not select
    // a walk frame while fading--this is for the bonus teleporter in L13.
    if (pdie) playframe = PDIEFRAME + pdieframe;
    else
    {
        if (pfrozentime) playframe = PFRAME_FROZEN + pfrozenframe;
        else if (pfiring) playframe = PFIREFRAME;
        else if (ppogostick) playframe = PFRAME_POGO + (pjumping==PPREPAREPOGO);
        else if (pjumping) playframe += pjumpframe;
        else if (pfalling) playframe += 13;
        else if (pwalking || playpushed_x) playframe += pwalkframe;
    }

    // if he's going left switch the frame selected above to the
    // appropriate one for the left direction
    if (pshowdir && !pdie && !pfrozentime)
    {
       if (pfiring)
       {
          playframe++;
       }
       else if (ppogostick)
       {
          playframe+=2;
       }
       else if (pjumping || pfalling)
       {
          playframe+=6;
       }
       else
       {
          playframe+=4;
       }
    }
}

void CPlayer::ankh()
{
/*int o;
  if (!ankhtime) return;

  o = ankhshieldobject;
  objects[o].x = x - (8<<CSF);
  objects[o].y = y - (8<<CSF);

  ankhtime--;
  if (!ankhtime)
    objects[o].exists = 0;

  else if (ankhtime < ANKH_STAGE3_TIME)
    objects[o].ai.se.state = ANKH_STATE_FLICKERSLOW;
  else if (ankhtime < ANKH_STAGE2_TIME)
    objects[o].ai.se.state = ANKH_STATE_FLICKERFAST;
  else
    objects[o].ai.se.state = ANKH_STATE_NOFLICKER;
*/
}

