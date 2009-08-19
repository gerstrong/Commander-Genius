/*
 * commonending.cpp
 *
 *  Created on: 07.08.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CommonEnding.h"

#include "../CGraphics.h"
#include "../sdl/CInput.h"
#include "../sdl/CTimer.h"

#include "../include/game.h"
#include "../include/gamedo.h"

#define LETTER_SHOW_SPD          30

#define HEADFOREARTH_WAIT_TIME  600


int endsequence(stCloneKeenPlus *pCKP)
{

  if (pCKP->Control.levelcontrol.episode==1)
  {
     if (eseq1_ReturnsToShip(pCKP)) return 0;
     if (eseq1_ShipFlys(pCKP,false)) return 0;
     if (eseq1_BackAtHome(pCKP)) return 0;
     if (eseq1_ShipFlys(pCKP,true)) return 0;
     eseq1_showEndingText(pCKP->Resources.GameDataDirectory);
     eseq_ToBeContinued();
  }
  else if (pCKP->Control.levelcontrol.episode==2)
  {
     if (eseq2_HeadsForEarth(pCKP)) return 0;
     if (eseq2_LimpsHome(pCKP)) return 0;
     if (eseq2_SnowedOutside(pCKP)) return 0;
  }
  else if (pCKP->Control.levelcontrol.episode==3)
  {
     if (eseq3_AwardBigV(pCKP)) return 0;
  }

  pCKP->Control.levelcontrol.gameovermode = true;
  // Remove all the items.

  start_gameover(pCKP);

  return 0;
}

// Just show a message like type writing. Maybe this one will be replaced by the new Dialog system
void eseq_showmsg(const std::string& text, int boxleft, int boxtop,
		int boxwidth, int boxheight, bool autodismiss)
{

	std::string tempbuf;
	char textline, showtimer;
	unsigned int amountshown;
	int waittimer;
	int cancel, lastcancelstate;

  textline = 0;
  amountshown = 0;
  showtimer = 0;
  lastcancelstate = 1;
  waittimer = 0;

  do
  {
	  if(g_pTimer->TimeToRunLogic())
	  {
		gamedo_fades();
		gamedo_AnimatedTiles();
		gamedo_render_drawobjects();

		cancel = (g_pInput->getPressedCommand(KENTER) || g_pInput->getPressedCommand(KCTRL) || g_pInput->getPressedCommand(KALT));

		// draw the text up to the amount currently shown
		tempbuf = text;
		if(amountshown < tempbuf.size())
		  tempbuf.erase(amountshown);
		sb_dialogbox(boxleft,boxtop,boxwidth,boxheight);
		g_pGraphics->sb_font_draw( tempbuf, (boxleft+1)*8, (boxtop+1+textline)*8);

		gamedo_render_eraseobjects();

		if (showtimer > LETTER_SHOW_SPD)
		{  // it's time to show a new letter
		  if (amountshown < text.size())
		  {
			amountshown++;
		  }
		  showtimer = 0;
		} else showtimer++;

		// user pressed enter or some other key
		if (cancel && !lastcancelstate)
		{
		  if (amountshown < text.size())
		  {
			 amountshown = text.size();
		  }
		  else return;
		}

		// when all text is shown wait a sec then return
		if (autodismiss)
		{
		  if (amountshown >= text.size())
		  {
			if (waittimer > HEADFOREARTH_WAIT_TIME) return;
			waittimer++;
		  }
		}


		lastcancelstate = cancel;

		g_pInput->pollEvents();
	  }
	  gamedo_frameskipping_blitonly();

  } while(!g_pInput->getPressedAnyCommand());
  return;
}

void eseq_ToBeContinued()
{
int i;
std::string text;
int dlgX, dlgY, dlgW, dlgH;

  // remove all objects because eseq_showmsg will call drawobjects
  for(i=0;i<MAX_OBJECTS;i++)
     objects[i].exists = 0;

  text = getstring("TO_BE_CONTINUED");
  dlgX = GetStringAttribute("TO_BE_CONTINUED", "LEFT");
  dlgY = GetStringAttribute("TO_BE_CONTINUED", "TOP");
  dlgW = GetStringAttribute("TO_BE_CONTINUED", "WIDTH");
  dlgH = GetStringAttribute("TO_BE_CONTINUED", "HEIGHT");
  eseq_showmsg(text, dlgX, dlgY, dlgW, dlgH, 0);

  do
  {
	  gamedo_fades();
	  if (g_pInput->getPressedKey(KQUIT)) return;
	  g_pInput->pollEvents();
  } while(fade.mode == FADE_GO);
}

