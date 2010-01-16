/*
 * CTimer.h
 *
 *  Created on: 12.08.2009
 *      Author: Caitlin Shaw
 */


#ifndef CTIMER_H_
#define CTIMER_H_

#include <SDL.h>
#include "../CSingleton.h"
#define g_pTimer	CTimer::Get()

#ifdef WIZ
#include "sys/wizgp2x.h"
#define timerTicks WIZ_ptimer_get_ticks_ms
#define timerDelay WIZ_ptimer_delay_ms
#else
#define timerTicks SDL_GetTicks
#define timerDelay SDL_Delay
#endif

#define MSPERSEC        1000
#define DEFAULT_SYNC    3
#define DEFAULT_LPS     60
#define DEFAULT_FPS     60

typedef unsigned long  ulong;

class CTimer : public CSingleton<CTimer>
{
public:
    CTimer();
    virtual ~CTimer();

    void CalculateIntervals( void );
    bool TimeToLogic();
    bool TimeToRender();
    void TimeToDelay();

    void ResetSecondsTimer();
    bool HasSecElapsed();
    bool HasTimeElapsed(int msecs);

    int getLogicRate() { return m_LogicRate; }
    int getFrameRate() { return m_FrameRate; }
    void setFrameRate( int logicrate, int framerate, int syncrate );

    int getLogicPerSec( void ) { return m_LPS; }
    int getFramesPerSec( void ) { return m_FPS; }

    Uint32 getTicks() { return timerTicks(); }

private:
    int m_LPS, m_LogicCount, m_LogicRate, m_LogicInterval;
    int m_FPS, m_FrameCount, m_FrameRate, m_FrameInterval;
    int m_LoopPS, m_LoopRate, m_LoopCount, m_LoopDuration;
    int m_SkipPS, m_FrameCountSkip, m_Ticks;
    int m_SyncCount, m_SyncRate, m_SyncDuration;
    bool m_FrameSkip;

    ulong m_LPSCountTime, m_FPSCountTime;
    ulong m_LoopStartTime, m_SyncStartTime;
    ulong m_LastSecTime;
};

#endif /* CTIMER_H_ */
