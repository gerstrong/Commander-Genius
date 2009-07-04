/*
 * CTimer.h
 *
 *  Created on: 26.03.2009
 *      Author: gerstrong
 */


#ifndef CTIMER_H_
#define CTIMER_H_

#include "../CSingleton.h"
#define g_pTimer	CTimer::Get()

#include <SDL.h>

void initTimer(void);

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

class CTimer : public CSingleton<CTimer>
{
public:
	CTimer();
	virtual ~CTimer();

	void SpeedThrottle(void);
	void RefreshThrottleFPS(void);

	uint8 getFrameskip(void){ return m_frameskip; }
	void setFrameskip(uint8 frameskip){ m_frameskip = frameskip; }

	uint64 getTime(void) { return ltime; }

private:

	uint64 ttime;
	uint64 ltime;
	int64 delay;

	uint64 m_timepoint;
	uint64 m_elaptime;
	uint8  m_frameskip;
};

#endif /* CTIMER_H_ */
