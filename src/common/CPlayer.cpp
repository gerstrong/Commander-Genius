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

	// When worldmap is set up, use that frame
	playframe = PMAPDOWNFRAME;

	hideplayer = false;
  	pwalkframe = 0;
   	m_player_number = 0;
   	mp_levels_completed = NULL;
   	mp_object = NULL;

    dpadcount = 0;
    hideplayer = false;
    mounted = false;
    ppogostick = false;
    pjumping = 0;
    pfalling = false;
    pwalking = playspeed = 0;
    m_speed_x = pinertia_y = 0;
    playpushed_x = 0;
    pfiring = 0;
    psliding = psemisliding = 0;
    pdie = 0;

    pfrozentime = 0;
    ankhtime = 0;
    keyprocstate = 0;         // KPROC_IDLE
    pjustjumped = pjustfell = 0;
    psupportingtile = psupportingobject = lastsupportingobject = 0;

    mp_map = NULL;
    mp_StatusScr = NULL;

    m_godmode = false;
    m_cheats_enabled = false;
    m_showStatusScreen = false;
}

// handles walking. the walking animation is handled by gamepdo_walkinganim()
void CPlayer::Walking()
{
	/*// Walk to the left or right!
	if (playcontrol[PA_X] > 0)
    { // RIGHT key down
     	  m_speed_x += playcontrol[PA_X];
    }
    else if (playcontrol[PA_X] < 0)
    { // LEFT key down
		  m_speed_x += playcontrol[PA_X];
    }*/
}

// animation for walking
void CPlayer::WalkingAnimation()
{
	if(!ppogostick && !pjumping && blockedu) // he is standing or walking on something
	{
		// Now Animation rate depends on player speed
		// Make him change frame if timer is up
		if(pwalkanimtimer >= 2400)
		{
			// increase walk frame and wrap it to 1st frame if needed
			if (pwalkframea < 4)
			{ pwalkframea++; }
			else
			{ pwalkframea=1; }

			// if this happens add speed points to the player
			m_speed_x += 512;
			pwalkanimtimer = 0;
		}
		else
			pwalkanimtimer += PLAYER_MAXSPEED;

        // set walk frame: map frame "4" to frame "2", this gives a
        // sequence of 1,2,3,2,1,2,3,2,1,2,3,2....
        if (pwalkframea==4) pwalkframe = 2;
        else pwalkframe = pwalkframea;
	}

    /*// no walk animation while sliding
    if (inhibitwalking || psliding ) return;

    // should we do walk animation?
    if (pwalking  || playpushed_x || psemisliding)
    {
    	int walkanimrate; // walk animation speed according to player speed

   		walkanimrate = PWALKANIMRATE;
			if(walkanimrate > 150)
        		walkanimrate = 150;

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
            	   g_pSound->playStereofromCoord(SOUND_KEEN_WALK, PLAY_NOW, mp_object->scrx);
                 }
               else
                 {
            	   g_pSound->playStereofromCoord(SOUND_KEEN_WALK2, PLAY_NOW, mp_object->scrx);
                 }

               if( m_playingmode != WORLDMAP && (blockedr || blockedl) )
               {
            	   g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->scrx);
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
            		   g_pSound->playStereofromCoord(SOUND_KEEN_BUMPHEAD, PLAY_NOW, mp_object->scrx);
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
        if (pwalkframea==4) pwalkframe = 2;
        else pwalkframe = pwalkframea;
    }*/
}

// handles inertia and friction for the X direction
// (this is where the inertia/playpushed_x is actually applied to playx)
void CPlayer::InertiaAndFriction_X()
{
   int friction;

   if (hideplayer)
   {
	 m_speed_x = 0;
     return;
   }
   // don't move when firing
   if (pfiring && !pjumping && !pfalling)
   {
	 m_speed_x = 0;
   }

   // Check walking boost and pogoing. It is similar to inertia
   if(pjumping || pfalling)
   {
	   if (playcontrol[PA_X] < 0 && !pfrozentime)
	   {
		   if( !ppogostick )
		   {
			   m_speed_x /= 2;
		   }
	   }
	   if (playcontrol[PA_X] > 0 && !pfrozentime)
	   {
		   if( !ppogostick )
		   {
			   m_speed_x /= 2;
		   }
	   }
   }

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
  else
  {
	  treshold = 100;
  }

  // if we hit a solid object do not move, and keep inertia...
  // * at slow speed: if we're falling or jumping and we're facing
  //                  the right direction, we want to keep
  //                  a small amount of inertia pent up so we can
  //                  easily get into tight spaces.
  // * at zero:       otherwise, or if we're not walking, we want
  //                  to hold the inertia at zero so we don't "shoot"
  //                  off of platforms, etc which looks weird.
  if (m_speed_x < 0 && blockedl)
  {
      if (!pwalking || (!pfalling && !pjumping) || pdir==RIGHT || (pfrozentime && m_episode==1))
      {
    	  m_speed_x = 0;
    	  widejump = false;
      }
      else if (m_speed_x < -PLAYER_FASTINCMAXSPEED)
      {
    	  m_speed_x /= 2;
      }
      return;
  }
  else if (m_speed_x > 0 && blockedr)
  {
	  if (!pwalking || (!pfalling && !pjumping) || pdir==LEFT || (pfrozentime && m_episode==1))
	  {
		  m_speed_x = 0;
		  widejump = false;
      }
      else if (m_speed_x > PLAYER_FASTINCMAXSPEED)
      {
    	  m_speed_x /= 2;
      }
   	  return;
  }

  pmaxspeed = treshold*PLAYER_MAXSPEED/100;

	  if(m_speed_x > pmaxspeed)
		  m_speed_x = pmaxspeed;
	  if(m_speed_x < -pmaxspeed)
		  m_speed_x = -pmaxspeed;

   // apply pinertia_x and playpushed_x inertia
   // (unless we're about to make a pogo jump)
   if ( pjumping != PPREPAREPOGO)
   {
	   int dx = m_speed_x + playpushed_x;
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
       friction = PFRICTION_RATE_WM;
     }
     else
     {
        if (!pfalling & !pjumping)
        {
          friction = PFRICTION_RATE_ONGROUND;
        }
        else
        {
          friction = PFRICTION_RATE_INAIR;
        }
     }

     // and apply friction to pinertia_x
     // when pogoing apply friction till we get down to PLAYER_FASTINCMAXSPEED
     // then stop the friction
     if ( !ppogostick || m_speed_x>PLAYER_FASTINCMAXSPEED || m_speed_x<-PLAYER_FASTINCMAXSPEED )
     {
         if ( !pfrozentime || m_episode != 1 )
         {  // disable friction while frozen
        	// here the wall animation must be applied!
            if (m_speed_x < 0)
            {
            	m_speed_x += friction;
            	if(m_speed_x>0) m_speed_x = 0;
            }
            else if (m_speed_x > 0)
            {
            	m_speed_x -= friction;
            	if(m_speed_x<0) m_speed_x = 0;
            }
         }
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
       	pinertia_y += PFRICTION_RATE_WM;
       	if(pinertia_y > 0) pinertia_y = 0;
      }
      else if (pinertia_y > 0)
      {
      	pinertia_y -= PFRICTION_RATE_WM;
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
