/*
 * CTimer.cpp
 *
 *  Created on: 26.03.2009
 *      Author: gerstrong
 */

//#include "../MathLib.h" // For what is that?

/* The timer (speed throttler) driver for SDL.
   This code is a slightly modified version
   of the code used in FCEU.
*/

#include <SDL.h>
#include "../keen.h"
#include "CTimer.h"
#include "CVideoDriver.h"
#include "../CLogFile.h"

CTimer::CTimer()
{
	RenderRate = (1000 / 60);			// 60fps
	LogicRate = (1000 / 333);			// 333fps
	g_pLogFile->textOut(GREEN, true, "Starting timer driver...\n");
	InitTimers();
}

CTimer::~CTimer()
{
}

void CTimer::InitTimers(void)
{
	LastRenderTime = LastLogicTime = LastSecTime = SDL_GetTicks();
}

bool CTimer::TimeToRunLogic(void)
{
	ulong CurTime = SDL_GetTicks();
	if ((CurTime - LastLogicTime) >= LogicRate)
	{
		LastLogicTime += LogicRate;
		return true;
	}
	else return false;
}

bool CTimer::TimeToRender(void)
{
ulong CurTime = SDL_GetTicks();
	if ((CurTime - LastRenderTime) >= RenderRate)
	{
		///LastRenderTime += RenderRate;
		LastRenderTime = CurTime;
		// do not render if we are behind on the logic
		if ((CurTime - LastLogicTime) > LogicRate)
			return false;

		if (m_FPS > 0)
			RenderRate = (1000 / m_FPS);
		return true;
	}
	else return 0;
}

//////////////////////////////////////////////////////////
// Those are for measuring the time in the game itself. //
//////////////////////////////////////////////////////////
void CTimer::ResetSecondsTimer(void)
{
	LastSecTime = SDL_GetTicks();
}

// will return nonzero once per second
bool CTimer::HasSecElapsed(void)
{
ulong CurTime = SDL_GetTicks();
	if (CurTime - LastSecTime >= 1000)
	{
		LastSecTime = CurTime;
		return true;
	}
	return false;
}
