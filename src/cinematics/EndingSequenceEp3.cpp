/* ESEQ_EP3.C
  Ending sequence for Episode 3.
  I never bothered to completely finish it...things like
  the Vorticon's camera flashes and stuff aren't implemented.
*/

#include "../keen.h"

#include "../include/game.h"
#include "../include/gamedo.h"
#include "../include/menu.h"

#include "../sdl/CInput.h"
#include "../sdl/CVideoDriver.h"
#include "../sdl/sound/CSound.h"

#include "EndingSequenceEp3.h"
#include "CommonEnding.h"

void eseq3_Mortimer()
{
int x,y,w,h;
	g_pSound->playSound(SOUND_MORTIMER, PLAY_FORCE);

  x = GetStringAttribute("EP3_MORTIMER", "LEFT");
  y = GetStringAttribute("EP3_MORTIMER", "TOP");
  w = GetStringAttribute("EP3_MORTIMER", "WIDTH");
  h = GetStringAttribute("EP3_MORTIMER", "HEIGHT");

  eseq_showmsg(getstring("EP3_MORTIMER"),x,y,w,h, false);
  eseq_showmsg(getstring("EP3_MORTIMER2"),x,y,w,h, false);
  eseq_showmsg(getstring("EP3_MORTIMER3"),x,y,w,h, false);
  eseq_showmsg(getstring("EP3_MORTIMER4"),x,y,w,h, false);
  eseq_showmsg(getstring("EP3_MORTIMER5"),x,y,w,h, false);

  map_redraw();
  g_pSound->playSound(SOUND_FOOTSLAM, PLAY_NOW);
}

char eseq3_AwardBigV(stCloneKeenPlus *pCKP)
{
  int x,y,w,h;
  int c;
  initgame( &(pCKP->Control.levelcontrol) );

  showmapatpos(81, 32, 32, pCKP);

  g_pInput->flushAll();

  numplayers = 1;
  player[0].x = 244<<CSF;
  player[0].y = 104<<CSF;
  player[0].playframe = 0;

  x = GetStringAttribute("EP3_ESEQ_PAGE1", "LEFT");
  y = GetStringAttribute("EP3_ESEQ_PAGE1", "TOP");
  w = GetStringAttribute("EP3_ESEQ_PAGE1", "WIDTH");
  h = GetStringAttribute("EP3_ESEQ_PAGE1", "HEIGHT");

  eseq_showmsg(getstring("EP3_ESEQ_PAGE1"),x,y,w,h, true);
  eseq_showmsg(getstring("EP3_ESEQ_PAGE2"),x,y,w,h, true);
  eseq_showmsg(getstring("EP3_ESEQ_PAGE3"),x,y,w,h, true);
  eseq_showmsg(getstring("EP3_ESEQ_PAGE4"),x,y,w,h, true);

  SDL_Surface *finale_sfc = SDL_CreateRGBSurface( g_pVideoDriver->BlitSurface->flags, 320, 200, 8, 0, 0, 0, 0);
  SDL_SetColors( finale_sfc, g_pGfxEngine->Palette.m_Palette, 0, 255);
  finale_draw( finale_sfc, "finale.ck3", pCKP->Resources.GameDataDirectory);
  scrollx_buf = scrolly_buf = 0;
  player[0].hideplayer = 1;

  x = GetStringAttribute("THE_END", "LEFT");
  y = GetStringAttribute("THE_END", "TOP");
  w = GetStringAttribute("THE_END", "WIDTH");
  h = GetStringAttribute("THE_END", "HEIGHT");

  eseq_showmsg(getstring("THE_END"),x,y,w,h, false, finale_sfc);

  // wait for enter pressed
  c = 0;
  do
  {
    if (c==0 && !g_pInput->getPressedCommand(IC_STATUS)) c++;
    if (c==1 && g_pInput->getPressedCommand(IC_STATUS)) c++;

    g_pInput->pollEvents();
    if (g_pInput->getPressedKey(KQUIT)) break;
  } while(1);

  SDL_FreeSurface(finale_sfc);

  if (g_pInput->getPressedKey(KQUIT)) return 1;
  return 0;
}
