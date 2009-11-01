/*
 * CTimer.h
 *
 *  Created on: 12.08.2009
 *      Author: Caitlin Shaw
 */


#ifndef CTIMER_H_
#define CTIMER_H_

#include "../CSingleton.h"
#define g_pTimer	CTimer::Get()

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
	
	int getLogicRate() { return m_LogicRate; }
	int getFrameRate() { return m_FrameRate; }
	void setFrameRate( int logicrate, int framerate, int syncrate );
	
    int getLogicPerSec( void ) { return m_LPS; }
	int getFramesPerSec( void ) { return m_FPS; }
	
private:
    int m_LPS, m_LogicCount, m_LogicRate, m_LogicInterval;
    int m_FPS, m_FrameCount, m_FrameRate, m_FrameInterval;
    int m_LoopPS, m_LoopRate, m_LoopCount, m_LoopDuration;
    int m_SyncCount, m_SyncRate, m_SyncDuration;
	
	
    ulong m_LPSCountTime, m_FPSCountTime;
	ulong m_LoopStartTime, m_SyncStartTime;
	ulong m_LastSecTime;
};

#endif /* CTIMER_H_ */
