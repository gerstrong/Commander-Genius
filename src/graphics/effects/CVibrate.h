/*
 * CVibrate.h
 *
 *  Created on: 14.12.2009
 *      Author: gerstrong
 */

#ifndef CVIBRATE_H_
#define CVIBRATE_H_

#include "CEffects.h"
#include "../../sdl/CTimer.h"

class CVibrate : public CEffects
{
public:
	CVibrate(Uint32 msecs);

	void process();

	virtual ~CVibrate();

private:
	Uint32 m_StartTime;
	Uint32 m_RunTime;

	char m_dir_x, m_dir_y;
};

#endif /* CVIBRATE_H_ */
