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
	bool TimeToRunLogic();
	bool TimeToRender();

	void ResetSecondsTimer();
	bool HasSecElapsed();

	int getFrameskip() { return m_FPS; }
	void setFrameskip(int value) { m_FPS=value; }

private:

	ulong LastRenderTime, LastLogicTime, LastSecTime;
	ulong RenderRate;			// 60fps
	ulong LogicRate;			// 333fps
	int m_FPS;				// This one must first be read
};

#endif /* CTIMER_H_ */
