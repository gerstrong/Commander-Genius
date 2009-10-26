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
A chunk factor is also used, this group a number of loops togther before delaying. The time normally
between these loops is minimal and the user does not notice if the loops are performed one after
enough. A delay is then performed after the loops are been executed to keep the chunk in sync.
The longer delay makes the needed accuracy of the timing system significantly less.
This also improves accuracy of the loops.

Examples:
NORMAL RATES
Frame Rate = 60
Logic Rate = 60

Loops Needed = 60
Normal duration: 16.66 ms Chunk of 3 duration: 50 ms

LOW RATES
Frame Rate = 20 (interval of 3)
Logic Rate = 60

Loops Needed = 60
Normal duration: 16.66 ms Chunk of 3 duration: 50 ms

HIGH RATES
Frame Rate = 120
Logic Rate = 60 (interval of 2)

Loops Needed = 120
Normal duration: 8.33 ms Chunk of 3 duration: 25 ms

*/

#include <SDL.h>
#include "../keen.h"
#include "CTimer.h"
#include "CVideoDriver.h"
#include "../CLogFile.h"
#include "../StringUtils.h"

CTimer::CTimer()
{
	m_LoopPS = m_LPS = m_FPS = 0;
	m_ChunkCount = m_LoopCount = m_LogicCount = m_FrameCount = 0;

	setFrameRate(DEFAULT_LPS, DEFAULT_FPS, DEFAULT_CHUNK);
    m_LoopStartTime = SDL_GetTicks();
	g_pLogFile->textOut(GREEN, true, "Starting timer driver...\n");
}

CTimer::~CTimer()
{
}

void CTimer::setFrameRate( int logicrate, int framerate, int chunkrate )
{
    int looprate, factor;

    // Set all of the desired rates
    m_ChunkRate = chunkrate;
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
    // Determine the number of loops needed for each logic and frame loop rates
    m_LogicInterval = m_LoopRate / m_LogicRate;
    m_FrameInterval = m_LoopRate / m_FrameRate;
    // Calculate the amount of time each loop chunk should last (floats keep from losing accuracy with integers)
    m_LoopDuration  = (int)(((float)MSPERSEC / (float)m_LoopRate) * (float)m_ChunkRate);

    //printf( "LoopRate %d LogicRate %d FrameRate %d\n", m_LoopRate, m_LogicRate, m_FrameRate );
    //printf( "LogicInt %d FrameInt %d LoopDur %d\n", m_LogicInterval, m_FrameInterval, m_LoopDuration );
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
    m_ChunkCount++;

    // If the chunk rate is met, check time took
    if (m_ChunkCount>=m_ChunkRate)
    {
        // Delay for the remaining time
        delay = m_LoopDuration - (int)(curtime - m_LoopStartTime);
        if (delay>0) {
            SDL_Delay(delay);
        }
        m_ChunkCount = 0;
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

        //printf( "LoopPS %d LPS %d FPS %d\n", m_LoopPS, m_LPS, m_FPS );
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
