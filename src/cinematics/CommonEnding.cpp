/*
 * commonending.cpp
 *
 *  Created on: 07.08.2009
 *      Author: gerstrong
 */

#include "../keen.h"
#include "CommonEnding.h"

#include "../CGraphics.h"
#include "../graphics/CGfxEngine.h"

#include "../sdl/CInput.h"
#include "../sdl/CVideoDriver.h"

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
		int boxwidth, int boxheight, bool autodismiss, SDL_Surface *spritesurface)
{
std::string tempbuf;
char showtimer;
unsigned int amountshown;
int waittimer;
int cancel, lastcancelstate;
SDL_Surface *sfc = g_pVideoDriver->FGLayerSurface;

  amountshown = 0;
  showtimer = 0;
  lastcancelstate = 1;
  waittimer = 0;

  do
  {
	  if(g_pGfxEngine->Palette.in_progress())
		  g_pGfxEngine->Palette.applyFade();
	  gamedo_AnimatedTiles();
	  gamedo_render_drawobjects();

	  cancel = (g_pInput->getPressedCommand(IC_STATUS) || g_pInput->getPressedCommand(IC_JUMP) || g_pInput->getPressedCommand(IC_POGO));

	  // draw the text up to the amount currently shown
	  tempbuf = text;
	  if(amountshown < tempbuf.size())
		  tempbuf.erase(amountshown);
	  if( spritesurface )
		  SDL_BlitSurface( spritesurface, NULL, g_pVideoDriver->BlitSurface, NULL );
	  g_pGfxEngine->drawDialogBox(sfc, boxleft,boxtop,boxwidth,boxheight);
	  g_pGfxEngine->Font->drawFont(sfc, tempbuf, (boxleft+1)*8, (boxtop+1)*8);

	  if (showtimer > LETTER_SHOW_SPD)
	  {  // it's time to show a new letter
		  if (amountshown < text.size())
			  amountshown++;

		  showtimer = 0;
	  } else showtimer++;

	  // user pressed enter or some other key
	  if (cancel)
	  {
		  if (amountshown < text.size())
			  amountshown = text.size();

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
	  gamedo_frameskipping_blitonly();
  } while(!g_pInput->getPressedCommand(KQUIT));
  return;
}

void eseq_ToBeContinued(SDL_Surface *extrascreen)
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
  eseq_showmsg(text, dlgX, dlgY, dlgW, dlgH, 0, extrascreen);
  do
  {
	  if (g_pInput->getPressedAnyCommand()) return;
	  g_pInput->pollEvents();
  } while(1);
}

