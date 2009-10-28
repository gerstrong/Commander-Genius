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
   if (!pdie) return;                // should never happen...
   if (pdie==PDIE_DEAD) return;      // if true animation is over
   if (pdie==PDIE_FELLOFFMAP)
   {
     // wait for falling sound to complete, then kill the player
     if (!g_pSound->isPlaying(SOUND_KEEN_FALL))
     {
        //pdie = false;
        //killplayer(cp);
    	// TODO: Not sure, what must go here! Check out!
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
     if (((y>>CSF)+96 > mp_map->m_scrolly) && (y>(16<<CSF)))
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
     }
   }
   else
   {  // not yet time to fly off screen, decrement timer
     pdietillfly--;
   }  // end "time to fly"
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

// handle playpushed_x: for yorps/scrubs/etc pushing keen
void CPlayer::playpushed()
{
    if (mp_option[OPT_CHEATS].value && g_pInput->getHoldedKey(KTAB)) return;

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
    }
}

// allow Keen to toggle the pogo stick and hit switches
void CPlayer::TogglePogo_and_Switches()
{
int i;
int mx, my, t;
CSprite *standsprite = g_pGfxEngine->Sprite[PSTANDFRAME];

	// detect if KPOGO key only pressed
	if (playcontrol[PA_POGO] && !lastpogo && !pfrozentime)
	{
		// if we are standing near a switch hit the switch instead
		mx = (x>>CSF)+(standsprite->getWidth()/2);

		for(i=standsprite->getHeight()-1;i>=0;i-=8)
		{
			my = (y>>CSF)+i;

			t = mp_map->at(mx, my);

			// check for extending-platform switch
			if (t==TILE_SWITCH_UP || t==TILE_SWITCH_DOWN )
			{
				//ExtendingPlatformSwitch(mx, my, p_levelcontrol);
				// TODO: ADD CODE here, but this must happen outside this function
				if (!ppogostick) return;
			}
			else if (t==TILE_LIGHTSWITCH)
			{ // lightswitch
				   /*p_levelcontrol->dark ^= 1;
				   g_pGfxEngine->Palette.setdark(p_levelcontrol->dark);
				   g_pSound->playStereofromCoord(SOUND_SWITCH_TOGGLE, PLAY_NOW, objects[useObject].scrx);
				if (!ppogostick) return;*/
				// TODO: ADD CODE here, but this must happen outside this function
			}
		}

		// toggle pogo stick
		if (inventory.HasPogo)
		{
			ppogostick ^= 1;
		}
		lastpogo = true;
	}
	else
	{
		lastpogo = false;
	}
}

void CPlayer::JumpAndPogo()
{
   // handle the JUMP key, both for normal jumps and (high) pogo jumps
   if (!pjumping && !pfalling && !pfiring)
   {
     // give em the chance to jump
     if (playcontrol[PA_JUMP] && !ppogostick && !pfrozentime)
     {
  	   pinertia_x = 0;
       pjumping = PPREPAREJUMP;
       pjumpframe = PPREPAREJUMPFRAME;
       pjumpanimtimer = 0;
       pwalking = false;
     }
     else if (ppogostick)
     {
  	   pinertia_x = 0;
       pjumping = PPREPAREPOGO;
       pjumpanimtimer = 0;
       pwalking = false;
     }
   }

    switch(pjumping)
    {
      case PPREPAREPOGO:
          if (pjumpanimtimer>PPOGO_PREPARE_TIME)
          {
             // continously bounce while pogo stick is out
        	  g_pSound->playStereofromCoord(SOUND_KEEN_JUMP, PLAY_NOW, mp_object->at(m_player_number).scrx);

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
					  pjumpupspeed_decrease = PJUMP_UPDECREASERATE_POGO_LONG;
				   }
				   else
				   {
					  pjumpupspeed = PPOGOUP_SPEED_SUPER;
					  pjumptime = PJUMP_NORMALTIME_POGO_LONG_SUPER;
					  pjumpupspeed_decrease = PJUMP_UPDECREASERATE_POGO_LONG_SUPER;
				   }
        	  }
        	  else
        	  {
        		  if(ppogostick)
        		  {
        			  pjumpupspeed = PJUMPUP_SPEED;
        			  pjumptime = PJUMP_NORMALTIME_POGO_SHORT;
        			  pjumpupspeed_decrease = PJUMP_UPDECREASERATE_POGO_SHORT;
        		  }
        	  }
        	  pjumpframe = PJUMP_PREPARE_LAST_FRAME;
        	  pjumping = PPOGOING;
        	  pjumpupspeed_decrease = 0;
        	  pjustjumped = 1;

          } else pjumpanimtimer++;
          break;
      case PPREPAREJUMP:
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
                            pjumpupspeed_decrease = PJUMP_UPDECREASERATE_6;
                            pjumpupspeed = 1;
                            break;
                       case PPREPAREJUMPFRAME+1:
                            pjumptime = PJUMP_NORMALTIME_5;
                            pjumpupspeed_decrease = PJUMP_UPDECREASERATE_5;
                            pjumpupspeed = 2;
                            break;
                       case PPREPAREJUMPFRAME+2:
                            pjumptime = PJUMP_NORMALTIME_4;
                            pjumpupspeed_decrease = PJUMP_UPDECREASERATE_4;
                            pjumpupspeed = 4;
                            break;
                       case PPREPAREJUMPFRAME+3:
                            pjumptime = PJUMP_NORMALTIME_3;
                            pjumpupspeed_decrease = PJUMP_UPDECREASERATE_3;
                            pjumpupspeed = 8;
                            break;
                       case PPREPAREJUMPFRAME+4:
                            pjumptime = PJUMP_NORMALTIME_2;
                            pjumpupspeed_decrease = PJUMP_UPDECREASERATE_2;
                            pjumpupspeed = 16;
                            break;
                       default:
                            pjumptime = PJUMP_NORMALTIME_1;
                            pjumpupspeed_decrease = PJUMP_UPDECREASERATE_1;
                            break;
                       }

                       pjumpframe = PJUMP_PREPARE_LAST_FRAME;

                       g_pSound->playStereofromCoord(SOUND_KEEN_JUMP, PLAY_NOW, mp_object->at(m_player_number).scrx);
                       pjumping = PJUMPUP;
                       pjustjumped = 1;

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

                       pwalkincreasetimer = 0;

                       if(playcontrol[PA_X] < 0)	pinertia_x = -80;
                       if(playcontrol[PA_X] > 0)	pinertia_x = 80;
                  }
                  else
                  {
                    pjumpframe++;
                  }
                  pjumpanimtimer=0;
             } else pjumpanimtimer++;
             break;
        case PJUMPUP:
        case PPOGOING:
        // check for hitting a ceiling
         if (blockedu)   // did we bonk something?
         {  // immediatly abort the jump
            pjumping = PNOJUMP;
            g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->at(m_player_number).scrx);
         }

         // do the jump
         if (!pjumptime)
         {
   		   if (pjumpupspeed <= 0)
   		   {
   			   pjumpupspeed = 0;
   			   pjumping = PNOJUMP;
   		   }
   		   else
   		   {
   			   pjumpupspeed-=pjumpupspeed_decrease;
   		   }
         }
         else pjumptime--;

         goto_y -= (2*pjumpupspeed);
         break;
    }

    // Now check how much the direction of the player is given
    if(pjumping)
    {
    	if (playcontrol[PA_X] < 0)
    		pinertia_x-=1;
    	if (playcontrol[PA_X] > 0)
			pinertia_x+=1;
    }

    if(pfalling)
    {
   		if(pinertia_x<0) pinertia_x+=2;
   		if(pinertia_x>0) pinertia_x-=2;

       	if (playcontrol[PA_X] < 0)
       		pinertia_x-=2;
       	if (playcontrol[PA_X] > 0)
   			pinertia_x+=2;
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
int canRefire;
CObject *pPlayerObject = &mp_object->at(m_player_number);

   if (pfireframetimer) pfireframetimer--;

   // FIRE button down, and not keencicled?
   if ( playcontrol[PA_FIRE] && !pfrozentime)
   { // fire is newly pressed

       inhibitwalking = 1;    // prevent moving
       pfiring = true;           // flag that we're firing
       ppogostick = false;        // put away pogo stick if out

       // limit how quickly shots can be fired
       if ( !plastfire || mp_option[OPT_FULLYAUTOMATIC].value )
       {
         if (pfireframetimer < PFIRE_LIMIT_SHOT_FREQ_FA) canRefire = true;
         else canRefire = false;
       }
       else
       {
         if (pfireframetimer < PFIRE_LIMIT_SHOT_FREQ) canRefire = true;
         else canRefire = false;
       }

       if (canRefire)
       {
          // show raygun for a minimum time even if FIRE is immediatly released
          pfireframetimer = PFIRE_SHOWFRAME_TIME;

          // try to fire off a blast
          if (inventory.charges)
          {  // we have enough charges
        	  int xdir, ydir;
        	  CObject rayobject;
        	  inventory.charges--;
        	  pshowdir = pdir;

        	  g_pSound->playStereofromCoord(SOUND_KEEN_FIRE, PLAY_NOW, pPlayerObject->scrx);

        	  ydir = y+(9<<(CSF-4));
        	  if (pdir==RIGHT) xdir = x+((g_pGfxEngine->Sprite[0]->getWidth()-4)<<(CSF-4));
        	  else xdir = x-(12<<(CSF-4));

        	  rayobject.spawn(xdir, ydir, OBJ_RAY);
        	  rayobject.ai.ray.direction = pdir;

    		  rayobject.ai.ray.dontHitEnable = false;
        	  if (!mp_option[OPT_ALLOWPKING].value)
        		  rayobject.ai.ray.dontHit = OBJ_PLAYER;
        	  mp_object->push_back(rayobject);
          }
          else
          { // oh shit, out of bullets
            // click!
        	  g_pSound->playStereofromCoord(SOUND_GUN_CLICK, PLAY_NOW, pPlayerObject->scrx);

          }  // end "do we have charges?"
       } // end "limit how quickly shots can be fired"
       plastfire = true;
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
      plastfire = false;
   }
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
int o;
  if (!ankhtime) return;

  o = ankhshieldobject;
  mp_object->at(o).x = x - (8<<CSF);
  mp_object->at(o).y = y - (8<<CSF);

  ankhtime--;
  if (!ankhtime)
	  mp_object->at(o).exists = 0;

  else if (ankhtime < ANKH_STAGE3_TIME)
	  mp_object->at(o).ai.se.state = ANKH_STATE_FLICKERSLOW;
  else if (ankhtime < ANKH_STAGE2_TIME)
	  mp_object->at(o).ai.se.state = ANKH_STATE_FLICKERFAST;
  else
	  mp_object->at(o).ai.se.state = ANKH_STATE_NOFLICKER;
}

