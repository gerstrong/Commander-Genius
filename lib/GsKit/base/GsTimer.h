/*
 * CTimer.h
 *
 *  Created on: 12.08.2009
 *      Author: Caitlin Shaw
 */


#ifndef CTIMER_H_
#define CTIMER_H_

#include <SDL.h>
#include <base/Singleton.h>

#define gTimer CTimer::get()

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

#if defined(ANDROID)
const float DEFAULT_FPS = 40.0f;
#else
const float DEFAULT_FPS = 60.0f;
#endif

#define ulong unsigned long

class CTimer : public GsSingleton<CTimer>
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

    float FPS() { return 1000.0f/mRenderLatency; }
    
    float LogicLatency() { return mLogicLatency; }

    float RenderLatency() { return mRenderLatency; }
    
    inline bool resetLogicSingal()
    { 
	if(resetLogic) 
	{
	    resetLogic = false;
	    return true;
	}
	return false;
    }
    
    void setLogicReset(const bool value)
    { resetLogic = value; }

    int getTicksPerFrame();

    Uint32 getTicks() { return timerTicks(); }        
    
    void setTimeforLastLoop(const float total_elapsed)
    { mtotalElapsed = total_elapsed; }
    
    float TimeForLastLoop()
    { return mtotalElapsed; }

    float LastFPS()
    { return 1000.0f/mtotalElapsed; }
    
    
private:
    
    float mRenderLatency;
    float mLogicLatency;
    
    float mtotalElapsed;
  
    ulong m_LastSecTime;
    
    bool resetLogic;
};

#endif /* CTIMER_H_ */
