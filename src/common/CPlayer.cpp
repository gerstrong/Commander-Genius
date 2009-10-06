/*
 * CPlayer.cpp
 *
 *  Created on: 05.10.2009
 *      Author: gerstrong
 */

#include "CPlayer.h"

#include "../keen.h"
#include "../sdl/CInput.h"
#include <stdlib.h>

///
// Initialization Part
///
CPlayer::CPlayer() {
	// When worldmap is set up, use that frame
	playframe = PMAPDOWNFRAME;
	hideplayer = false;
	pwalkframe = 0;
	m_player_number = 0;
	m_levels_completed = NULL;
}

///
// Process Part
///

////
// Process the stuff of the player when playing on the world map
void CPlayer::processWorldMap()
{
    inhibitwalking = false;
    inhibitfall = false;

    //gamepdo_StatusBox(cp, pCKP);

    ProcessInput();
    setWorldMapdir();

    setWMblockedlrud();
    //gamepdo_wm_AllowEnterLevel(cp, pCKP);

    Walking();
    //gamepdo_walkinganim(cp, pCKP);

    //if (fade.mode==NO_FADE)
    //{
      //gamepdo_InertiaAndFriction_X(cp, pCKP);
      //gamepdo_InertiaAndFriction_Y(cp, pCKP);
    //}

    /*if (pCKP->Control.levelcontrol.episode==3)
    {
      AllowMountUnmountNessie(cp);
    }*/

    selectFrameOnWorldMap();
}

// select proper player direction
void CPlayer::setWorldMapdir()
{
bool exception;
   dpadcount = 0;
   if (playcontrol[PA_X] < 0) { pdir = LEFT; dpadcount++; }
   if (playcontrol[PA_X] > 0) { pdir = RIGHT; dpadcount++; }
   if (playcontrol[PA_Y] < 0) { pdir = UP; dpadcount++; }
   if (playcontrol[PA_Y] > 0) { pdir = DOWN; dpadcount++; }

   exception = false;
   if ( pshowdir==UP && !playcontrol[PA_Y] < 0) exception = true;
   if ( pshowdir==DOWN && !playcontrol[PA_Y] > 0) exception = true;
   if ( pshowdir==LEFT && !playcontrol[PA_X] < 0) exception = true;
   if ( pshowdir==RIGHT && !playcontrol[PA_X] > 0) exception = true;
   if ((playcontrol[PA_Y] < 0) && (playcontrol[PA_Y] > 0)) pshowdir = DOWN;
   if ((playcontrol[PA_X] < 0) && (playcontrol[PA_X] > 0)) pshowdir = RIGHT;

   if (dpadcount==1 || dpadlastcount==0 || exception)
   {
     pshowdir = pdir;
   }
   dpadlastcount = dpadcount;
}

// set blockedl and blockedr...is Keen up against a solid object?
void CPlayer::setWMblockedlrud()
{
   blockedl = blockedr = 0;
   blockedu = blockedd = 0;

   // cheat: holding down TAB will turn off clipping. or if you are in godmode
   //if ((pCKP->Option[OPT_CHEATS].value && g_pInput->getHoldedKey(KTAB)) || player[cp].godmode) return;

   // R
   if (isWMSolid((x>>CSF)+8, (y>>CSF)+1, m_levels_completed))
      { blockedr = true; }
   else if (isWMSolid((x>>CSF)+8, (y>>CSF)+8, m_levels_completed))
      { blockedr = true; }
   else if (isWMSolid((x>>CSF)+8, (y>>CSF)+13, m_levels_completed))
      { blockedr = true; }

   // L
   if (isWMSolid((x>>CSF)+0, (y>>CSF)+1, m_levels_completed))
      { blockedl = true; }
   else if (isWMSolid((x>>CSF)+0, (y>>CSF)+8, m_levels_completed))
      { blockedl = true; }
   else if (isWMSolid((x>>CSF)+0, (y>>CSF)+13, m_levels_completed))
      { blockedl = true; }

   // U
   if (isWMSolid((x>>CSF)+1, (y>>CSF)-1, m_levels_completed))
      { blockedu = true; }
   else if (isWMSolid((x>>CSF)+4, (y>>CSF)-1, m_levels_completed))
      { blockedu = true; }
   else if (isWMSolid((x>>CSF)+7, (y>>CSF)-1, m_levels_completed))
      { blockedu = true; }

   // D
   if (isWMSolid((x>>CSF)+1, (y>>CSF)+14, m_levels_completed))
      { blockedd = true; }
   else if (isWMSolid((x>>CSF)+4, (y>>CSF)+14, m_levels_completed))
      { blockedd = true; }
   else if (isWMSolid((x>>CSF)+7, (y>>CSF)+14, m_levels_completed))
      { blockedd = true; }

}



bool CPlayer::isWMSolid(int xb, int yb, bool *levels_completed)
{
/*
  // for map tiles solidl and solidr are always gonna be the same...
  // so we can get away with this.
  if ( TileProperty[getmaptileat(xb, yb)][BLEFT] ) return true;
  //if (tiles[getmaptileat(xb, yb)].solidl) return 1;

  unsigned int level;
  level = getlevelat(xb, yb);

  if (level & 0x8000)
  {
	  if(levels_completed[map.objectlayer[xb>>4][yb>>4] & 0x7fff] && options[OPT_LVLREPLAYABILITY].value) // check if level is done, but can be replayed
		  return 0;

	  if(g_pInput->getHoldedKey(KTAB) && g_pInput->getHoldedKey(KSHIFT))
	  {
		  return 0;
	  }
	  else
	  {
		  return 1;
	  }
  }*/
  return false;
}

// handles walking. the walking animation is handled by gamepdo_walkinganim()
void CPlayer::Walking()
{
	int cur_pfastincrate;
    if (inhibitwalking && !psliding)
    {
      if (!pfrozentime||m_episode!=1)
        if (!pjumping && !pfalling)
          pinertia_x = 0;
      return;
    }

    // this prevents a "slipping" effect if you jump, say, right, then
    // start walking left just as you hit the ground
    if (pjustjumped && ((pinertia_x > 0 && pdir==LEFT) ||\
                        (pinertia_x < 0 && pdir==RIGHT)))\
    {
    	if(!ppogostick)
    		pinertia_x = 0;
    }

    // this code makes it so that if you jump/fall onto a semi-sliding
    // block you'll start moving a little
    if (!pjumping && !pfalling)
    {
      // on left/right press clear pjustjumped
      if ((playcontrol[PA_X] < 0)||(playcontrol[PA_X] > 0))
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
    if ((psemisliding && pinertia_x!=0) || (((playcontrol[PA_X] < 0) || (playcontrol[PA_X] > 0) || (((playcontrol[PA_Y] < 0) || (playcontrol[PA_Y] > 0))&&map.isworldmap)) && !inhibitwalking))
    {
      // we just started walking or we changed directions suddenly?
      if (pwalking == 0 || ((lastpdir==RIGHT && pdir==LEFT)||(lastpdir==LEFT && pdir==RIGHT)))
      {
    	widejump = false;
        pwalkanimtimer = 0;
        pwalkframe = 1;
        pwalkincreasetimer = 0;
        pfriction_timer_x = 0;
        pfriction_timer_y = 0;

        if (!pjumping && !pfalling)
        {
            if(!ppogostick) // Only if he stays on the ground (No pogoing)
          		pinertia_x /= 2;
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
             pinertia_x = PMAXSPEED;
           }
           else if (pdir==LEFT)
           {
             pinertia_x = -PMAXSPEED;
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

	  if((pjumpdir != UP) && (pjumping != 0))
	  {
		  return;
	  }

      if (playcontrol[PA_X] > 0)
      { // RIGHT key down

          // quickly reach PFASTINCMAXSPEED
          if (pwalkincreasetimer>=cur_pfastincrate && pinertia_x<PFASTINCMAXSPEED)
          {
             pinertia_x++;
             pwalkincreasetimer=0;
          }
          else
          {
             pwalkincreasetimer++;
          }

          // prevent sliding on map
          if (map.isworldmap )
          {
        	  if(pinertia_x < 0)
        	  {
        		  pinertia_x = 0;
        	  }
        	  else
        	  {
        		  pinertia_x = playcontrol[PA_X]*PFASTINCMAXSPEED/100;
        	  }
          }

          // increase up to max speed every time frame is changed
          if (!pwalkanimtimer && pinertia_x < PMAXSPEED)
          {
             pinertia_x++;
          }
      }
      else if (playcontrol[PA_X] < 0)
      {
          // quickly reach PFASTINCMAXSPEED
          if (pwalkincreasetimer>=cur_pfastincrate && pinertia_x>-PFASTINCMAXSPEED)
          {
             pinertia_x--;
             pwalkincreasetimer=0;
          }
          else
          {
             pwalkincreasetimer++;
          }

          // prevent sliding on map
          if (map.isworldmap )
          {
        	  if(pinertia_x > 0)
        	  {
        		  pinertia_x = 0;
        	  }
        	  else
        	  {
        		  pinertia_x=playcontrol[PA_X]*PFASTINCMAXSPEED/100;
        	  }
          }
          // increase up to max speed every time frame is changed
          if (!pwalkanimtimer && pinertia_x>-PMAXSPEED)
          {
             pinertia_x--;
          }
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
          if (map.isworldmap )
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
          if (map.isworldmap )
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

void CPlayer::selectFrameOnWorldMap()
{
    // select base frame for current direction
    if (pshowdir==RIGHT) playframe = PMAPRIGHTFRAME;
    else if (pshowdir==LEFT) playframe = PMAPLEFTFRAME;
    else if (pshowdir==UP) playframe = PMAPUPFRAME;
    else if (pshowdir==DOWN) playframe = PMAPDOWNFRAME;

    // episode 3 map frames start at 31, ep1&2 at 32
    if (m_episode==3) playframe--;

    // add in walk frame if walking
    if (pwalking) playframe += pwalkframe;
}

////
// Process the stuff of the player when playing in a normal level
void CPlayer::processInLevel()
{

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

		 memset(player[cp].playcontrol,0,PA_MAX_ACTIONS);

	      player[cp].inhibitfall = 1;
	     if (pCKP->Control.levelcontrol.level_done==LEVEL_DONE_WALK)
	     {
	        // keep him going right
	        player[cp].pdir = player[cp].pshowdir = RIGHT;
	        // make keen walk slowly through the exit door
	        player[cp].playcontrol[PA_X] = 80;
	        if (player[cp].pinertia_x > PMAXEXITDOORSPEED)
	        {
	          player[cp].pinertia_x = PMAXEXITDOORSPEED;
	        }
	     }
	     else
	     {
	        // he's all the way through the door; hold him still
	        player[cp].pinertia_x = 0;
	        player[cp].pwalking = 0;
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
		   if(player[cp].playcontrol[PA_JUMP] && player[cp].playcontrol[PA_POGO])
		   {
			   player[cp].playcontrol[PA_FIRE] = 1;
			   player[cp].playcontrol[PA_JUMP] = 0;
			   player[cp].playcontrol[PA_POGO] = 0;
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

///
// Cleanup Part
///
CPlayer::~CPlayer() {
	// TODO Auto-generated destructor stub
}
