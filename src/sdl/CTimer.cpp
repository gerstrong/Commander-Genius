/*
 * CTimer.cpp
 *
 *  Created on: 26.03.2009
 *      Author: gerstrong
 */


#include "../keen.h"
#include "../keenext.h"
#include <SDL.h>

#include "CTimer.h"

typedef char int8;
typedef short int16;
typedef long  int32;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

#ifdef __GNUC__
 typedef unsigned long long uint64;
 typedef long long int64;
 #define INLINE inline
 #define GINLINE inline
#endif

Uint32 FPSCallback(Uint32 interval, void* param);

static uint64 tfreq;
static uint64 desiredfps;

SDL_TimerID ckp_timer_id;

Uint32 FPSCallback(Uint32 interval, void* param)
{
  fps = (curfps << 1);
  curfps = 0;
  return interval;
}

CTimer::CTimer() {
	  RefreshThrottleFPS();
	  if(SDL_Init(SDL_INIT_TIMER) == 0)
		  ckp_timer_id = SDL_AddTimer(1000, FPSCallback, NULL);
}

CTimer::~CTimer() {
	SDL_RemoveTimer(ckp_timer_id);
}

void CTimer::SpeedThrottle(void)
{
	 static uint64 ttime,ltime=0;

	 waiter:

	 ttime=SDL_GetTicks();
	 ttime*=10000;

	 if( (ttime-ltime) < (tfreq/desiredfps) )
	 {
	  int64 delay;
	  delay=(tfreq/desiredfps)-(ttime-ltime);
	  if(delay>0)
	   SDL_Delay(delay/10000);
	  goto waiter;
	 }
	 if( (ttime-ltime) >= (tfreq*4/desiredfps))
	  ltime=ttime;
	 else
	  ltime+=tfreq/desiredfps;
}
void CTimer::RefreshThrottleFPS(void)
{
//       desiredfps=FCEUI_GetDesiredFPS()>>8;
//        desiredfps = 23000000;
       desiredfps = 21800000;
       tfreq=10000000;
       tfreq<<=16;    /* Adjustment for fps returned from FCEUI_GetDesiredFPS(). */
}

