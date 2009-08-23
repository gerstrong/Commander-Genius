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

#define LOGIC_LPS	333			// loops per second (keep as factor of FPS)
#define RENDER_FPS	60

CTimer::CTimer()
{	
	// Exmaple: if we loop logic at 300 times per sec and we want to draw 60 frames per sec then 300/60 is how many logic loops must pass until
	//          a frame can be drawn. To maximize time we can do 300/60 loops back to back and then draw the frame, to sync to our desired rates
	//	    we can delay the remaining time it would take to do (1000/300)*(300/60) ms
	//	    Visual exmaple: logic->logic->frame->delay(left over time from 2 logic loops)
	m_FPSRate = RENDER_FPS;	// frames per second
	CalculateRate();
	
	g_pLogFile->textOut(GREEN, true, "Starting timer driver...\n");
	InitTimers();
}

CTimer::~CTimer()
{
}

void CTimer::setFrameRate(int value)
{
	m_FPSRate=value;
	CalculateRate();
}

void CTimer::CalculateRate( void )
{
	m_RenderInterval	= LOGIC_LPS/m_FPSRate;				// loops per frame
	m_LogicRateMS		= (1000.0f/(float)LOGIC_LPS)*m_RenderInterval;	// millsec for each loops per frame group
}

void CTimer::InitTimers(void)
{
	m_FramesTotal	= 0;
	m_LoopsTotal	= 0;
	m_RenderIntervalCount = 0;
	m_CountTime = m_LogicRenderStart = SDL_GetTicks();
}

bool CTimer::TimeToRender(void)
{
	bool render;

	m_LoopsTotal++;
	m_RenderIntervalCount++;

	// Should a frame be drawn?
	if( m_RenderIntervalCount >= m_RenderInterval )
	{
		m_RenderIntervalCount = 0;	// Restart the count
		m_FramesTotal++;
		render = true;
	} else {
		render = false;
	}

	return render;
}

void CTimer::TimeToDelay( void )
{
	signed int delay;
	ulong curtime = SDL_GetTicks();
    
	if( curtime > m_LogicRenderStart )
	{
		delay = m_LogicRateMS - (int)(curtime - m_LogicRenderStart);
		if( delay>0 && delay<(m_LogicRateMS*2) ) {
			SDL_Delay(delay);
		}
/*
		do {
		    curtime = SDL_GetTicks();
		} while( (curtime - m_LogicRenderStart) <= m_LogicRateMS );
*/
	}

	// Display the loops per second and the frames drawn
	if( curtime - m_CountTime >= 1000 || m_CountTime > curtime  )
	{
		//printf( "LPS %d FPS %d Interval %d MSrate %d\n", m_LoopsTotal, m_FramesTotal, m_RenderInterval, m_LogicRateMS  );
		m_FramesTotal	= 0;
		m_LoopsTotal	= 0;
		m_CountTime = curtime;
	}

	m_LogicRenderStart = SDL_GetTicks();
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
	if (CurTime - m_LastSecTime >= 1000)
	{
		m_LastSecTime = CurTime;
		return true;
	}
	return false;
}
