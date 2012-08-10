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
	m_FPSCountTime = m_LoopPS = m_LPS = m_FPS = 0;
	m_SyncCount = m_LoopCount = m_LogicCount = m_FrameCount = 0;
	m_FrameCountSkip = m_SkipPS = 0;
	m_FrameSkip = false;
	m_Ticks = 0;

	setRates(DEFAULT_LPS, DEFAULT_FPS, DEFAULT_SYNC);
#if defined(WIZ)
	WIZ_ptimer_init();
#endif
	m_SyncStartTime = m_LoopStartTime = timerTicks();
	g_pLogFile->textOut(GREEN, true, "Starting timer driver...\n");
}

void CTimer::setRates( int logicrate, int framerate, int syncrate )
{
	// Check limits
	if (logicrate <= 0)
		logicrate = DEFAULT_LPS;

	if (framerate <= 0)
		framerate = DEFAULT_FPS;

	// Set all of the desired rates
	m_SyncRate  = syncrate;
	m_LogicRate = logicrate;
	m_FrameRate = framerate;

	// Find the smallest number that both rates divide,
	// or lcm(logicrate, framerate).

	// Find a number that is factor for both rates
	// Old Code by Scott (Pickle)
	/*for ( factor=0; factor<=10; factor++ )
	{
		looprate = m_LoopRate+(20*factor);
		if (looprate%m_LogicRate==0 && looprate%m_FrameRate==0)
			break;
	}
	m_LoopRate = looprate;*/

	while (logicrate != framerate)
	{
		if (logicrate < framerate)
			logicrate += m_LogicRate;
		else
			framerate += m_FrameRate;
	}
	m_LoopRate = logicrate;

	CalculateIntervals();
}


void CTimer::setFPS( const int framerate )
{
	setRates(DEFAULT_LPS, framerate, DEFAULT_SYNC);
}


void CTimer::CalculateIntervals()
{
	float looprate;

	// Determine the number of loops needed for each logic and frame loop rates
	m_LogicInterval = m_LoopRate / m_LogicRate;
	m_FrameInterval = m_LoopRate / m_FrameRate;
	// Calculate the amount of time each loop Sync should last (floats keep from losing accuracy with integers)
	looprate = (float)MSPERSEC / (float)m_LoopRate;
	m_LoopDuration  = (int)looprate;
	looprate = looprate * (float)m_SyncRate;
	m_SyncDuration  = (int)looprate;

	g_pLogFile->ftextOut( "LoopRate %d LogicRate %d FrameRate %d\n", m_LoopRate, m_LogicRate, m_FrameRate );
	g_pLogFile->ftextOut( "LogicInt %d FrameInt %d LoopDur %d SyncDur %d\n", m_LogicInterval, m_FrameInterval, m_LoopDuration, m_SyncDuration );
}

bool CTimer::TimeToLogic()
{
	bool result = false;

	if (m_LoopCount % m_LogicInterval == 0)
	{
		result = true;
		m_LogicCount++;
	}

	return result;
}

bool CTimer::TimeToRender()
{
	bool result = false;

	if (m_FrameSkip == true)
	{
		m_FrameCountSkip++;
	}
	else
	{
		if (m_LoopCount % m_FrameInterval == 0)
		{
			result = true;
			m_FrameCount++;
		}
	}

	return result;
}

void CTimer::TimeToDelay( void )
{
	signed int duration;
	signed int starttime;
	signed int delay;

	unsigned int curtime = timerTicks();
	m_LoopCount++;
	m_SyncCount++;

	// If the Sync rate is met, check time took
	if (m_SyncCount>=m_SyncRate)
	{
		duration = m_SyncDuration;
		starttime = m_SyncStartTime;
	}
    else
    {
    	duration = m_LoopDuration;
    	starttime = m_LoopStartTime;
    }

    // Delay for the remaining time
    delay = duration + m_Ticks - (signed int)(curtime - starttime);
    // Cap the delay
    if (delay>duration)
    {
    	delay = duration/4;
    }

    if (delay>=0)
    {
    	m_Ticks = 0;
    	m_FrameSkip = false;

    	if (delay>0)
    		timerDelay(delay);
    }
    else if (delay<0)
    {
    	m_FrameSkip = true;
    	m_Ticks += (signed int)(curtime - starttime) - duration;
    }

    // If the Sync rate is met, check time took
    if (m_SyncCount>=m_SyncRate)
    {
    	m_SyncCount = 0;
    	m_SyncStartTime = m_LoopStartTime = timerTicks();
    }
    else
    {
    	m_LoopStartTime = timerTicks();
    }    

    // Display the loops/logic/frames per second
    if( curtime - m_FPSCountTime >= MSPERSEC  )
    {
    	m_LoopPS = m_LoopCount;
    	m_LPS    = m_LogicCount;
    	m_FPS    = m_FrameCount;
    	m_SkipPS = m_FrameCountSkip;
    	m_LoopCount = m_LogicCount = m_FrameCount = m_FrameCountSkip = 0;
    	m_FPSCountTime = curtime;
    	/*
	// FrameCap Limit Down
	if (delay>duration/2)
	    setRates(DEFAULT_LPS, m_FrameRate+20, DEFAULT_SYNC);

	// FrameCap Limit Down
	if (m_SkipPS>m_FrameRate/4)
	    setRates(DEFAULT_LPS, m_FrameRate-20, DEFAULT_SYNC);
    	 */
#ifdef DEBUG
    	//g_pLogFile->ftextOut( "LOOP %d LPS %d FPS %d Skip %d\n", m_LoopPS, m_LPS, m_FPS, m_SkipPS );
#endif
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
