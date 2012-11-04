/*
 * CTimer.h
 *
 *  Created on: 12.08.2009
 *      Author: Caitlin Shaw
 */


#ifndef CTIMER_H_
#define CTIMER_H_

#include <SDL.h>
#include "CSingleton.h"
#define g_pTimer	CTimer::Get()

#if defined(WIZ)
#include "sys/wizgp2x.h"
#define timerTicks WIZ_ptimer_get_ticks_ms
#define timerDelay WIZ_ptimer_delay_ms
#else
#define timerTicks SDL_GetTicks
#define timerDelay SDL_Delay
#endif

#define MSPERSEC        1000
const float DEFAULT_LPS_GALAXY = 70.0f;
const float DEFAULT_LPS_VORTICON = 60.0f;
#define DEFAULT_FPS     60

typedef unsigned long  ulong;

class CTimer : public CSingleton<CTimer>
{
public:
    CTimer();
    virtual ~CTimer();
#if 0
    void CalculateIntervals( void );
#endif
    void ResetCounters();
    bool TimeToRender();
    void TimeToDelay();

    void ResetSecondsTimer();
    bool HasSecElapsed();
    bool HasTimeElapsed(int msecs);

    //float getFrameRate() { return m_FrameRate; }
    //void setFPS( const int framerate );
    void setFPS( const float fps );    
    void setLPS( const float lps );   

    float FPS() { return 1.0f/mRenderLatency; }
    
    float LogicLatency() { return mLogicLatency; }

    float RenderLatency() { return mRenderLatency; }

    int getTicksPerFrame();

    Uint32 getTicks() { return timerTicks(); }
    
    /*float Logic2FPSratio() const
    {	return mLogic2FPSratio;	}*/
    

private:
    
    float mRenderLatency;
    float mLogicLatency;
  
    //float m_FrameRate, m_FrameDuration;
    //int mLogicSpeed;
    //int m_FPS, m_FrameCount;

    //ulong m_FrameUpdateTime;
    //ulong m_FPSCountTime;
    ulong m_LastSecTime;
    
    //float mLogic2FPSratio;
};

#endif /* CTIMER_H_ */
