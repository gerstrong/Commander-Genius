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

typedef unsigned long  ulong;

class CTimer : public CSingleton<CTimer>
{
public:
	CTimer();
	virtual ~CTimer();

	void InitTimers();
	void CalculateRate( void );
	bool TimeToRender();
	void TimeToDelay();

	void ResetSecondsTimer();
	bool HasSecElapsed();

	int getFrameRate() { return m_FPSRate; }
	void setFrameRate(int value);

private:

	ulong m_LogicRenderStart, m_CountTime, m_LastSecTime;
	int m_LoopsTotal;
	int m_FramesTotal;
	int m_LogicRateMS;
	int m_FPSRate;
	int m_RenderInterval;
	int m_RenderIntervalCount;
};

#endif /* CTIMER_H_ */
