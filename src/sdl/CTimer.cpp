/*
 * CTimer.cpp
 *
 *  Created on: 26.03.2009
 *      Author: gerstrong
 */

/* The timer (speed throttler) driver for SDL.
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
#include "../keen.h"
#include "CTimer.h"
#include "CVideoDriver.h"
#include "../CLogFile.h"
#include "../StringUtils.h"

CTimer::CTimer()
{
	m_FPSCountTime = m_LoopPS = m_LPS = m_FPS = 0;
	m_SyncCount = m_LoopCount = m_LogicCount = m_FrameCount = 0;

	setFrameRate(DEFAULT_LPS, DEFAULT_FPS, DEFAULT_SYNC);
    m_SyncStartTime = m_LoopStartTime = SDL_GetTicks();
	g_pLogFile->textOut(GREEN, true, "Starting timer driver...\n");
}

void CTimer::setFrameRate( int logicrate, int framerate, int syncrate )
{
    int looprate, factor;

    // Set all of the desired rates
    m_SyncRate  = syncrate;
    m_LogicRate = logicrate;
	m_FrameRate = framerate;

    // Check limits
	if (m_LogicRate <= 0) {
        m_LogicRate = DEFAULT_LPS;
	}
	if (m_FrameRate <= 0) {
        m_FrameRate = DEFAULT_FPS;
	}

    // Pick highest rate
	if (m_FrameRate >= m_LogicRate) {
        m_LoopRate = m_FrameRate;
	} else {
        m_LoopRate = m_LogicRate;
	}

    // Find a number that is factor for both rates
    for (factor=0; factor<=10; factor++ )
    {
        looprate = m_LoopRate+(20*factor);
        if (looprate%m_LogicRate==0 && looprate%m_FrameRate==0)
            break;
    }
    m_LoopRate = looprate;

	CalculateIntervals();
}

void CTimer::CalculateIntervals( void )
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

    //printf( "LoopRate %d LogicRate %d FrameRate %d\n", m_LoopRate, m_LogicRate, m_FrameRate );
    //printf( "LogicInt %d FrameInt %d LoopDur %d SyncDur %d\n", m_LogicInterval, m_FrameInterval, m_LoopDuration, m_SyncDuration );
}

bool CTimer::TimeToLogic( void )
{
    bool result;

    result = false;
    if (m_LoopCount % m_LogicInterval == 0) {
        result = true;
        m_LogicCount++;
    }

    return result;
}

bool CTimer::TimeToRender( void )
{
    bool result;

    result = false;
    if (m_LoopCount % m_FrameInterval == 0) {
        result = true;
        m_FrameCount++;
    }

    return result;
}

void CTimer::TimeToDelay( void )
{
	signed int delay;
	ulong curtime = SDL_GetTicks();

    m_LoopCount++;
    m_SyncCount++;

    // If the Sync rate is met, check time took
    if (m_SyncCount>=m_SyncRate)
    {
        // Delay for the remaining time
        delay = m_SyncDuration - (int)(curtime - m_SyncStartTime);
        if (delay>0) {
            SDL_Delay(delay);
        }
        m_SyncCount = 0;
        m_SyncStartTime = m_LoopStartTime = SDL_GetTicks();
    }
    else
    {
        // Delay for the remaining time
        delay = m_LoopDuration - (int)(curtime - m_LoopStartTime);
        if (delay>0) {
            SDL_Delay(delay);
        }
        m_LoopStartTime = SDL_GetTicks();
    }

	// Display the loops/logic/frames per second
	if( curtime - m_FPSCountTime >= MSPERSEC  )
	{
	    m_LoopPS        = m_LoopCount;
        m_LPS           = m_LogicCount;
		m_FPS           = m_FrameCount;
		m_LoopCount = m_LogicCount = m_FrameCount = 0;
		m_FPSCountTime  = curtime;

        printf( "LoopPS %d LPS %d FPS %d\n", m_LoopPS, m_LPS, m_FPS );
	}
}

//////////////////////////////////////////////////////////
// Those are for measuring the time in the game itself. //
//////////////////////////////////////////////////////////
void CTimer::ResetSecondsTimer(void)
{
	m_LastSecTime = SDL_GetTicks();
}

// will return nonzero once per second
bool CTimer::HasSecElapsed(void)
{
    ulong CurTime = SDL_GetTicks();
	if (CurTime - m_LastSecTime >= MSPERSEC)
	{
		m_LastSecTime = CurTime;
		return true;
	}
	return false;
}

CTimer::~CTimer()
{
}
