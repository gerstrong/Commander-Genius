/*
 * CPlayer.cpp
 *
 *  Created on: 05.10.2009
 *      Author: gerstrong
 *
 *  The Player logics are processed here.
 *  Some functions are common, others map and level specific.
 *  Those are stored in CPlayerWM.cpp and CPlayerLevel.cpp
 *
 */

#include "CPlayer.h"

#include "../keen.h"
#include "../sdl/CInput.h"
#include "../sdl/sound/CSound.h"
#include <stdlib.h>

///
// Initialization Part
///
CPlayer::CPlayer() {
	// Set every value in the class to zero.

   	mp_levels_completed = NULL;
   	mp_object = NULL;
    mp_map = NULL;
    mp_StatusScr = NULL;
    setDatatoZero();
}

// Sets the player data to the initial value
void CPlayer::setDatatoZero()
{
	// When worldmap is set up, use that frame
	playframe = PMAPDOWNFRAME;

	godmode = hideplayer = false;
  	pwalkframe = pwalkframea = 0;
   	m_player_number = 0;
    dpadcount = 0;
    hideplayer = false;
    mounted = false;
    ppogostick = false;
    plastfire = false;
    blockedu = blockedd = blockedl = blockedr = false;

    pjumping = pjumptime = 0;

    pfalling = false;
    pwalking = playspeed = 0;
    pinertia_x = pinertia_y = 0;
    playpushed_x = 0;
    plastfire = pfiring = false;
    psliding = psemisliding = 0;
    pwalkanimtimer = 0;
    inhibitfall = false;
    playerbaseframe = 0;
    mapplayx = mapplayy = 0;
    pdie = 0;

    pfrozentime = 0;
    ankhtime = 0;
    keyprocstate = 0;         // KPROC_IDLE
    pjustjumped = pjustfell = true;
    pfireframetimer = 0;
    psupportingtile = psupportingobject = lastsupportingobject = 0;

    m_godmode = false;
    m_cheats_enabled = false;
    m_showStatusScreen = false;
    lastpogo = false;

    // Set all the inventory to zero.
    memset(&inventory, 0, sizeof(stInventory));
    inventory.extralifeat = 20000;
}

// handles walking. the walking animation is handled by gamepdo_walkinganim()
void CPlayer::Walking()
{
	int cur_pfastincrate;
    if (inhibitwalking && !psliding)
    {
      if (!pfrozentime||m_episode!=1)
        if (!pjumping && !pfalling);
        	//pinertia_x = 0;
      return;
    }

    // this prevents a "slipping" effect if you jump, say, right, then
    // start walking left just as you hit the ground
    if (pjustjumped && ((pinertia_x > 0 && pdir==LEFT) ||\
                        (pinertia_x < 0 && pdir==RIGHT)))\
    {
    	if(!ppogostick);
    		//pinertia_x = 0;
    }

    // this code makes it so that if you jump/fall onto a semi-sliding
    // block you'll start moving a little
    if (!pjumping && !pfalling)
    {
      // on left/right press clear pjustjumped
      if ( (playcontrol[PA_X] < 0) || (playcontrol[PA_X] > 0) )
      {
        pjustjumped = 0;
        pjustfell = 0;
      }

      // if we fall onto a semislide tile with no inertia
      // don't move!.
      if (pjustfell && psemisliding)
      {
        if (pdir==RIGHT)
        {
          if (blockedr)
          {
            pjustjumped = 0;
            pjustfell = 0;
          }
          else
          {
            pshowdir = pdir;
          }
        }
        else
        {
          if (blockedl)
          {
            pjustjumped = 0;
            pjustfell = 0;
          }
          else
          {
            pshowdir = pdir;
          }
        }
      }
    }

    // test if we're trying to walk
    if ((psemisliding && pinertia_x!=0) || (((playcontrol[PA_X] < 0) || (playcontrol[PA_X] > 0) || (( (playcontrol[PA_Y] < 0) || (playcontrol[PA_Y] > 0)) && m_playingmode == WORLDMAP )) && !inhibitwalking))
    {
      // we just started walking or we changed directions suddenly?
      if (pwalking == 0 || ((lastpdir==RIGHT && pdir==LEFT)||(lastpdir==LEFT && pdir==RIGHT)))
      {
        pwalkanimtimer = 0;
        pwalkframe = 1;
        pwalkincreasetimer = 0;

        if (!pjumping && !pfalling)
        {
            if(!ppogostick) // Only if he stays on the ground (No pogoing)
            	//pinertia_x /= 2;
          pinertia_y = 0;
        }

        lastpdir = pdir;
        pwalking = 1;
      }
    }
    else
    {   // end "d-pad down and bit sliding"
    	if(pinertia_x != 0)
    		pslowingdown=1;
    	else
    	{
    		pslowingdown=0;
    		pwalking = 0;
    	}
    }

      // when sliding on ice force maximum speed
      if (psliding)
      {
         if (pjumping != PPREPAREJUMP &&
             pjumping != PPREPAREPOGO)
         {
           // reset walk frame because we have no walk animation while on ice
           pwalkframe = 0;
           // keep player sliding at maximum speed
           if (pdir==RIGHT)
           {
        	   //pinertia_x = PMAXSPEED;
           }
           else if (pdir==LEFT)
           {
        	   //pinertia_x = -PMAXSPEED;
           }
         }
         return;
      }
      else if (!pwalking) return;    // don't run rest of sub if not walking
      // if we get here we're walking and not sliding

     // increase player inertia while walk key held down
     if (ppogostick)
        cur_pfastincrate = PFASTINCRATE_POGO;
      else
        cur_pfastincrate = PFASTINCRATE;

	  /*if((pjumpdir != UP) && (pjumping != 0))
	  {
		  return;
	  }*/

      if (playcontrol[PA_X] > 0)
      { // RIGHT key down
          // quickly reach PLAYER_FASTINCMAXSPEED
          if (pwalkincreasetimer>=cur_pfastincrate && pinertia_x<PFASTINCMAXSPEED)
          {
        	  pinertia_x+=(1<<4);
        	  //pinertia_x++;
        	  pwalkincreasetimer=0;
          }
          else	pwalkincreasetimer++;

          // prevent sliding on map
          if ( m_playingmode == WORLDMAP )
          {
        	  if(pinertia_x < 0)  pinertia_x = 0;
        	  else	pinertia_x = playcontrol[PA_X]*PFASTINCMAXSPEED/100;
          }

          // increase up to max speed every time frame is changed
          if (!pwalkanimtimer && pinertia_x < PMAXSPEED)	pinertia_x+=(1<<4);
      }
      else if (playcontrol[PA_X] < 0)
      { // LEFT key down
          // quickly reach PFASTINCMAXSPEED
          if (pwalkincreasetimer>=cur_pfastincrate && pinertia_x<-PFASTINCMAXSPEED)
          {
        	  pinertia_x-=(1<<4);
        	  //pinertia_x--;
        	  pwalkincreasetimer=0;
          }
          else	pwalkincreasetimer++;

          // prevent sliding on map
          if ( m_playingmode == WORLDMAP )
          {
        	  if(pinertia_x > 0)  pinertia_x = 0;
        	  else	pinertia_x = playcontrol[PA_X]*PFASTINCMAXSPEED/100;
          }

          // decrease down to max speed every time frame is changed
          if (!pwalkanimtimer && pinertia_x > -PMAXSPEED)	pinertia_x-=(1<<4);
      }

      if (playcontrol[PA_Y] > 0)
      {
          // quickly reach PFASTINCMAXSPEED
          if (pwalkincreasetimer>=PFASTINCRATE && pinertia_y<PFASTINCMAXSPEED)
          {
             pinertia_y++;
             pwalkincreasetimer=0;
          }
          else
          {
             pwalkincreasetimer++;
          }
          // increase up to max speed every time frame is changed
          if (!pwalkanimtimer && pinertia_y<PMAXSPEED)
          {
             pinertia_y++;
          }

          // prevent sliding on map
          if ( m_playingmode == WORLDMAP )
          {
        	  if(pinertia_y < 0)
        	  {
        		  pinertia_y = 0;
        	  }
        	  else
        	  {
        		  pinertia_y=playcontrol[PA_Y]*PFASTINCMAXSPEED/100;
        	  }
          }

      }
      else if (playcontrol[PA_Y] < 0)
      {
          // quickly reach PFASTINCMAXSPEED
          if (pwalkincreasetimer>=PFASTINCRATE && pinertia_y>-PFASTINCMAXSPEED)
          {
             pinertia_y--;
             pwalkincreasetimer=0;
          }
          else
          {
             pwalkincreasetimer++;
          }
          // increase up to max speed every time frame is changed
          if (!pwalkanimtimer && pinertia_y>-PMAXSPEED)
          {
             pinertia_y--;
          }

          // prevent sliding on map
          if ( m_playingmode == WORLDMAP )
          {
        	  if(pinertia_y > 0)
        	  {
        		  pinertia_y = 0;
        	  }
        	  else
        	  {
        		  pinertia_y=playcontrol[PA_Y]*PFASTINCMAXSPEED/100;
        	  }
          }
      }
}

// animation for walking
void CPlayer::WalkingAnimation()
{
    // no walk animation while sliding
    if (inhibitwalking || psliding ) return;

    // should we do walk animation?
    if (pwalking  || playpushed_x || psemisliding)
    {
    	int walkanimrate; // walk animation speed according to player speed

    	if(!psemisliding)
    	{
    		walkanimrate = 101*PWALKANIMRATE/(treshold+1);
        	if(walkanimrate > 150)
        			walkanimrate = 150;
    	}
    	else
    		walkanimrate = PWALKANIMRATE;

    	// ** do walk animation **
        if (pwalkanimtimer > walkanimrate)
        { // time to change walking frame
          // make walk noise
          if (!pjumping && !pfalling)
          {
            if (!pfrozentime && pwalking)
            {
               if (pwalkframea&1)
                 {
            	   g_pSound->playStereofromCoord(SOUND_KEEN_WALK, PLAY_NOW, mp_object->at(m_player_number).scrx);
                 }
               else
                 {
            	   g_pSound->playStereofromCoord(SOUND_KEEN_WALK2, PLAY_NOW, mp_object->at(m_player_number).scrx);
                 }

               if( m_playingmode != WORLDMAP && (blockedr || blockedl) )
               {
            	   g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->at(m_player_number).scrx);
				   // It is not bumping a head, but walking in some direction and being blocked
               }
               else if ( m_playingmode == WORLDMAP )
               {
            	   // Same on world map!

            	   short play=0;

            	   if (blockedu && pdir == UP)
            		   play=1;

            	   if (blockedd && pdir == DOWN)
            		   play=1;

            	   if (blockedl && pdir == LEFT)
            		   play=1;

            	   if (blockedr && pdir == RIGHT)
            		   play=1;

            	   if (play)
            		   g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->at(m_player_number).scrx);
               }

            }
          }
          // increase walk frame and wrap it to 1st frame if needed
          if (pwalkframea < 4)
            { pwalkframea++; }
          else
            { pwalkframea=1; }

          pwalkanimtimer = 0;
        }
        else
        {  // did not change walk frame
          pwalkanimtimer++;
        }

        // set walk frame: map frame "4" to frame "2", this gives a
        // sequence of 1,2,3,2,1,2,3,2,1,2,3,2....
        if (pwalkframea==4)
        {
          pwalkframe = 2;
        }
        else
        {
          pwalkframe = pwalkframea;
        }
    }
}

// handles inertia and friction for the X direction
// (this is where the inertia/playpushed_x is actually applied to playx)
void CPlayer::InertiaAndFriction_X()
{
	   int friction_rate;

	   // Calculate Threshold of your analog device for walking animation speed!
	   if(!pfrozentime)
		   treshold = playcontrol[PA_X];

	  int pmaxspeed = 0;

	  if(!pjumping && !pfalling &&
		 !psemisliding && !psliding && !ppogostick && !pslowingdown)
	  {
		  if(treshold < playcontrol[PA_Y] && playcontrol[PA_Y] > 0 )
			  treshold = playcontrol[PA_Y];

		  if(treshold > playcontrol[PA_Y] && playcontrol[PA_Y] < 0 )
			  treshold = playcontrol[PA_Y];

		  if(treshold < 0)
			  treshold *= (-1);
	  }
	  else	treshold = 100;

	    pmaxspeed = treshold*PMAXSPEED/100;

	    if(!pjumping)
	    {
	    	if(pinertia_x > pmaxspeed)
	    		pinertia_x = pmaxspeed;
	    	if(pinertia_x < -pmaxspeed)
	    		pinertia_x = -pmaxspeed;
	    }

	   // apply pinertia_x and playpushed_x inertia
	   // (unless we're about to make a pogo jump)
	   if ( pjumping != PPREPAREPOGO && pjumping != PPREPAREJUMP)
	   {
		   int dx=pinertia_x + playpushed_x;
		   // check first if the player is not blocked
		   if( (!blockedr and dx>0) or (!blockedl and dx<0) )
			   goto_x += dx;
	   }

	   // if we stopped walking (i.e. left or right not held down) apply friction
	   // there's no friction if we're semisliding
	   if (!(playcontrol[PA_X] < 0) && !(playcontrol[PA_X] > 0) && \
	       !psemisliding)
	   {
	     // determine friction rate--different rates for on ground and in air
	     if (m_playingmode == WORLDMAP)
	     {
	       friction_rate = PFRICTION_RATE_WM;
	     }
	     else
	     {
	        if (!pfalling & !pjumping)
	        {
	          friction_rate = PFRICTION_RATE_ONGROUND;
	        }
	        else
	        {
	          friction_rate = PFRICTION_RATE_INAIR;
	        }
	     }


	     // and apply friction to pinertia_x
	     // when pogoing apply friction till we get down to PFASTINCMAXSPEED
	     // then stop the friction
	     if (!ppogostick || (pinertia_x > PFASTINCMAXSPEED) || (pinertia_x < -PFASTINCMAXSPEED))
	     {
	        if (pfriction_timer_x > friction_rate)
	        {
	          if (!pfrozentime || m_episode!=1)
	          {  // disable friction while frozen
	        	  // here the wall animation must be applied!
        		  if (pinertia_x < 0)
        		  {
        			  pinertia_x+=(1<<5);
	        		  if (pinertia_x > 0) pinertia_x = 0;
	        	  }
	        	  else if (pinertia_x > 0)
	        	  {
	        			  pinertia_x-=(1<<5);
	        			  if (pinertia_x < 0) pinertia_x = 0;
	        	  }
	          }

	          pfriction_timer_x = 0;
	        }
	        else pfriction_timer_x++;
	     }
	   }
}

// handles inertia and friction for the Y direction
// (this is where the inertia is actually applied to playx)
void CPlayer::InertiaAndFriction_Y()
{
   if (hideplayer)
   {
     pinertia_y = 0;
     return;
   }

   // if we hit a solid object kill all inertia
   if (pinertia_y > 0 && blockedd)
   {
     pinertia_y = 0;
   }
   else if (pinertia_y < 0 && blockedu)
   {
     pinertia_y = 0;
   }

   // apply pinertia_y
   goto_y += pinertia_y;

	// if we stopped walking (i.e. LRUD not held down) apply friction
    if (playcontrol[PA_Y] == 0)
    {
      // and apply friction to pinertia_y
      if (pinertia_y < 0)
      {
       	pinertia_y += PFRICTION_RATE_WM<<4;
       	if(pinertia_y > 0) pinertia_y = 0;
      }
      else if (pinertia_y > 0)
      {
      	pinertia_y -= PFRICTION_RATE_WM<<4;
      	if(pinertia_y < 0) pinertia_y = 0;
      }
    }
}

void CPlayer::ProcessInput()
{
	 // Entry for every player
	playcontrol[PA_X] = 0;
	playcontrol[PA_Y] = 0;

	if(g_pInput->getHoldedCommand(m_player_number, IC_LEFT))
		 playcontrol[PA_X] -= 100;
	if(g_pInput->getHoldedCommand(m_player_number, IC_RIGHT))
		 playcontrol[PA_X] += 100;

	if(g_pInput->getHoldedCommand(m_player_number, IC_UP))
		 playcontrol[PA_Y] -= 100;
	if(g_pInput->getHoldedCommand(m_player_number, IC_DOWN))
		 playcontrol[PA_Y] += 100;

	if(g_pInput->getHoldedCommand(m_player_number, IC_JUMP))
		 playcontrol[PA_JUMP]++;
	else
		 playcontrol[PA_JUMP] = 0;

		 playcontrol[PA_POGO]   = g_pInput->getHoldedCommand(m_player_number, IC_POGO)   ? 1 : 0;
		 playcontrol[PA_FIRE]   = g_pInput->getHoldedCommand(m_player_number, IC_FIRE)   ? 1 : 0;
		 playcontrol[PA_STATUS] = g_pInput->getHoldedCommand(m_player_number, IC_STATUS) ? 1 : 0;

		 // The possibility to charge jumps. This is mainly used for the pogo.
		 if( playcontrol[PA_JUMP] > 50) playcontrol[PA_JUMP] = 50;

	/*stLevelControl *p_levelcontrol;

		p_levelcontrol = &(pCKP->Control.levelcontrol);

	   // are we doing the keen-walking-through-exit door animation?
	   if (pCKP->Control.levelcontrol.level_done &&
			   pCKP->Control.levelcontrol.level_finished_by==cp)
	   {
	     // don't let player control keen

		 memset(playcontrol,0,PA_MAX_ACTIONS);

	      inhibitfall = 1;
	     if (pCKP->Control.levelcontrol.level_done==LEVEL_DONE_WALK)
	     {
	        // keep him going right
	        pdir = pshowdir = RIGHT;
	        // make keen walk slowly through the exit door
	        playcontrol[PA_X] = 80;
	        if (pinertia_x > PMAXEXITDOORSPEED)
	        {
	          pinertia_x = PMAXEXITDOORSPEED;
	        }
	     }
	     else
	     {
	        // he's all the way through the door; hold him still
	        pinertia_x = 0;
	        pwalking = 0;
	     }

	     if (p_levelcontrol->level_done_timer > LEVEL_DONE_TOTAL_WAIT_TIME)
	     {
	       if (p_levelcontrol->level_done != LEVEL_DONE_FADEOUT)
	       {
	    	   p_levelcontrol->level_done = LEVEL_DONE_FADEOUT;
	    	   endlevel(1, &(pCKP->Control.levelcontrol));
	    	   p_levelcontrol->chglevelto = WM_MAP_NUM;
	    	   p_levelcontrol->command = LVLC_CHANGE_LEVEL;
	    	   p_levelcontrol->levels_completed[p_levelcontrol->curlevel] = 1;
	       }
	     }
	     else if (p_levelcontrol->level_done_timer > LEVEL_DONE_STOPWALKING_TIME)
	     {
	    	 p_levelcontrol->level_done = LEVEL_DONE_WAIT;
	     }

	     p_levelcontrol->level_done_timer++;
	     return;
	   }

	   if(options[OPT_TWOBUTTON].value)
	   {
		   if(playcontrol[PA_JUMP] && playcontrol[PA_POGO])
		   {
			   playcontrol[PA_FIRE] = 1;
			   playcontrol[PA_JUMP] = 0;
			   playcontrol[PA_POGO] = 0;
		   }
	   }

	   if(g_pInput->getPressedKey(KP))
	   {
	       g_pSound->playSound(SOUND_GUN_CLICK, PLAY_FORCE);

		   // Open the Pause Dialog
	       showTextMB("Game Paused");
	   }
	   else if(g_pInput->getPressedKey(KF1))
	   {
		   // Show the typical F1 Help
	       showF1HelpText(pCKP->Control.levelcontrol.episode, pCKP->Resources.GameDataDirectory);
	   }*/
}

void CPlayer::StatusBox()
{
  if( g_pInput->getPressedCommand(IC_STATUS) )
	  m_showStatusScreen=true; // PlayGame class detects that variable and launches drawStatusScreen()
}

// Draws the Status screen and return false, when it's still open.
bool CPlayer::drawStatusScreen()
{
	if(m_showStatusScreen)
	{
		if(mp_StatusScr == NULL)
			mp_StatusScr = new CStatusScreen(m_episode, &inventory);

		mp_StatusScr->draw();

		if(g_pInput->getPressedAnyCommand())
		{
			m_showStatusScreen = false;
			delete mp_StatusScr;
			mp_StatusScr = NULL;
		}
		return false;
	}
	else return true;
}

///
// Cleanup Part
///
CPlayer::~CPlayer() {
	// TODO Auto-generated destructor stub
}
