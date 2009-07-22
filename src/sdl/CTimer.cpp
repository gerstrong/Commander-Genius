/*
 * CTimer.cpp
 *
 *  Created on: 26.03.2009
 *      Author: gerstrong
 */


#include "../keen.h"
#include "../keenext.h"
#include "../MathLib.h"

#include "CTimer.h"
#include "CVideoDriver.h"

Uint32 FPSCallback(Uint32 interval, void* param);

static uint64 tfreq;
static uint64 desiredfps;

SDL_TimerID ckp_timer_id;

Uint32 FPSCallback(Uint32 interval, void* param)
{
  fps = (curfps << 1);
  curfps = 0;

  unsigned char targetfps;
  targetfps = g_pVideoDriver->getTargetFPS();

  if(targetfps > 0 && fps > 0)
  {
	  if(fps > targetfps + 10)
	  {
		  uint8 fsk;
		  fsk = g_pVideoDriver->getFrameskip();

		  if(fsk < 20)
			  fsk++;

		  g_pVideoDriver->setFrameskip(fsk);
	  }
	  else if( fps < targetfps - 10)
	  {
		  uint8 fsk;
		  fsk = g_pVideoDriver->getFrameskip();

		  if(fsk > 0)
			  fsk--;

		  g_pVideoDriver->setFrameskip(fsk);
	  }
  }

  return interval;
}

CTimer::CTimer() {
	  RefreshThrottleFPS();
	  if(SDL_Init(SDL_INIT_TIMER) == 0)
		  ckp_timer_id = SDL_AddTimer(1000, FPSCallback, NULL);

	  ltime = 0;
	  m_frameskip = 0;
}

CTimer::~CTimer() {
	SDL_RemoveTimer(ckp_timer_id);
}

void CTimer::SpeedThrottle(void)
{
	//regulateFrameskip();

	 waiter:

	 ttime=SDL_GetTicks();
	 //ttime*=10000;
	 ttime<<=14;

	 if( (ttime-ltime) < (tfreq/desiredfps) )
	 {
		 delay=(tfreq/desiredfps)-(ttime-ltime);
		 Uint32 d = delay >> 14;
		 if(d>0) SDL_Delay(d);
		 goto waiter;
	 }
	 if( (ttime-ltime) >= ((tfreq<<2)/desiredfps) )
	  ltime=ttime;
	 else
	  ltime+=tfreq/desiredfps;
}
void CTimer::RefreshThrottleFPS(void)
{
//       desiredfps=FCEUI_GetDesiredFPS()>>8;
//        desiredfps = 23000000;
       //desiredfps = 21800000;
	   desiredfps = 13600000;
       tfreq=10000000;
       tfreq<<=16;    /* Adjustment for fps returned from FCEUI_GetDesiredFPS(). */
}
