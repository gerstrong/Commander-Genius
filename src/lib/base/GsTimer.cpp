/*
 * CTimer.cpp
 *
 * \Created on: 26.03.2009
 * \Authors: gerstrong, Scott, NY00123
 */

#include <SDL.h>
#include <base/GsLogging.h>

#include "GsTimer.h"

CTimer::CTimer() :
mRenderLatency(1000.0f/DEFAULT_FPS),
mLogicLatency(1000.0f/DEFAULT_LPS_VORTICON),
m_LastSecTime(0),
resetLogic(false)
{
#if defined(WIZ)
	WIZ_ptimer_init();
#endif
	setFPS(DEFAULT_LPS_VORTICON);
	gLogging.textOut(GREEN, true, "Starting timer driver...\n");
    ResetSecondsTimer();
}


void CTimer::setFPS( const float fps )
{
    mRenderLatency = 1000.0f / static_cast<float>(fps);
}

void CTimer::setLPS( const float lps )
{
    mLogicLatency = 1000.0f / static_cast<float>(lps);
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
