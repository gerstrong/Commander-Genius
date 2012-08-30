/*
 * CTimer.cpp
 *
 *  Created on: 26.03.2009
 *      Author: gerstrong
 */

/** The timer (speed throttler) driver for SDL.
 This timer calculates a common factor between the desired rates for logic and render. By having this
 common rate all of the time during one second can be divided into a measurement scale where both
 other rates can be triggered.
 A sync rate is used to sync a group of frames exactly to the desired frame rate.

 Examples:
 NORMAL RATES
 Frame Rate = 60
 Logic Rate = 60

 Loops Needed = 60
 Normal duration: 16.66 ms Sync of 3 duration: 50 ms

 LOW RATES
 Frame Rate = 20 (interval of 3)
 Logic Rate = 60

 Loops Needed = 60
 Normal duration: 16.66 ms Sync of 3 duration: 50 ms

 HIGH RATES
 Frame Rate = 120
 Logic Rate = 60 (interval of 2)

 Loops Needed = 120
 Normal duration: 8.33 ms Sync of 3 duration: 25 ms

 */

#include <SDL.h>
#include "CTimer.h"
#include "CVideoDriver.h"
#include "CLogFile.h"
#include "StringUtils.h"

CTimer::CTimer()
{
#if defined(WIZ)
	WIZ_ptimer_init();
#endif
	setRates(DEFAULT_LPS, DEFAULT_FPS);
	g_pLogFile->textOut(GREEN, true, "Starting timer driver...\n");
}



void CTimer::ResetCounters()
{
	m_FPSCountTime = m_FPS = 0;
	m_FrameCount = 0;
	// Update times for logic and rendering
	ulong curtime = timerTicks();
	// m_LogicUpdateTime is measured in time units defined as follows:
	// MSPERSEC is the time for a single logic "tick".
	m_LogicUpdateTime = curtime*m_LogicRate;
	// Similarly, for m_FrameUpdateTime, MSPERSEC is the time for a frame.
	m_FrameUpdateTime = curtime*m_FrameRate;
}


void CTimer::setRates( const unsigned int logicrate,
						  const unsigned int framerate)
{
	// Set all of the desired rates
	m_LogicRate = logicrate;
	m_FrameRate = framerate;

	// Check limits
	if (m_LogicRate <= 0)
		m_LogicRate = DEFAULT_LPS;

	if (m_FrameRate <= 0)
		m_FrameRate = DEFAULT_FPS;

	ResetCounters();
}


void CTimer::setFPS( const int framerate )
{
	setRates(DEFAULT_LPS, framerate);
}

#if 0
void CTimer::CalculateIntervals()
{
	m_LogicDuration = MSPERSEC / m_LogicRate;
	m_FrameDuration = MSPERSEC / m_FrameRate;

	g_pLogFile->ftextOut( "LogicDuration %d FrameDuration %d\n", m_LogicDuration, m_FrameDuration );
}
#endif

// Returns the amount of logic "ticks" that we should process
int CTimer::TimeToLogic()
{
	int result = 0;
	ulong curtime = timerTicks()*m_LogicRate;

	if ((signed)(curtime - m_LogicUpdateTime) >= 0)
	{
		result = (curtime-m_LogicUpdateTime)/MSPERSEC+1;
		m_LogicUpdateTime += result*MSPERSEC;
		//m_LogicUpdateTime = curtime+MSPERSEC;
	}

	return result;
}

bool CTimer::TimeToRender()
{
	bool result = false;
	ulong curtime = timerTicks()*m_FrameRate;

	if ((signed)(curtime - m_FrameUpdateTime) >= 0)
	{
		result = true;
		m_FrameCount++;
		//m_FrameUpdateTime += MSPERSEC;
		m_FrameUpdateTime += ((curtime-m_FrameUpdateTime)/MSPERSEC+1)*MSPERSEC;
		//m_FrameUpdateTime = curtime;
		//m_FrameUpdateTime = curtime+MSPERSEC;
#if 0
		if ((signed)(curtime - m_FrameUpdateTime) >= 0)
			m_FrameUpdateTime = curtime;
#endif
	}

	return result;
}

void CTimer::TimeToDelay( void )
{
	// Free some CPU cycles
	timerDelay(1);
	// Update the FPS counter
	ulong curtime = timerTicks();
	if (curtime - m_FPSCountTime >= MSPERSEC)
	{
		m_FPS = m_FrameCount;
		m_FrameCount = 0;
		m_FPSCountTime = curtime;
	}
}


int CTimer::getTicksPerFrame()
{
	int ratio = m_LogicRate/m_FrameRate;

	if(ratio < 1)
		ratio = 1;

	return ratio;
}


//////////////////////////////////////////////////////////
// Those are for measuring the time in the game itself. //
//////////////////////////////////////////////////////////
void CTimer::ResetSecondsTimer(void)
{
	m_LastSecTime = timerTicks();
}

// will return nonzero once per second
bool CTimer::HasSecElapsed(void)
{
	unsigned int CurTime = timerTicks();

	if ((signed int)(CurTime - m_LastSecTime) >= MSPERSEC)
	{
		m_LastSecTime = CurTime;
		return true;
	}
	return false;
}

bool CTimer::HasTimeElapsed(int msecs)
{
	unsigned int CurTime = timerTicks();

	if ((signed int)(CurTime - m_LastSecTime) >= msecs)
	{
		m_LastSecTime = CurTime;
		return true;
	}
	return false;
}

CTimer::~CTimer()
{
#if defined(WIZ)
	WIZ_ptimer_cleanup();
#endif
}
