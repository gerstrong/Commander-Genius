/*
 * CTimer.h
 *
 *  Created on: 26.03.2009
 *      Author: gerstrong
 */


#ifndef CTIMER_H_
#define CTIMER_H_

#include "../CSingleton.h"
#define g_pTimer	CTimer::Get()

void initTimer(void);

class CTimer : public CSingleton<CTimer>
{
public:
	CTimer();
	virtual ~CTimer();

	void SpeedThrottle(void);
	void RefreshThrottleFPS(void);

private:
};

#endif /* CTIMER_H_ */
