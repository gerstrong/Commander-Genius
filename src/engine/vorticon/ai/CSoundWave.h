/*
 * CSoundWave.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CSOUNDWAVE_H_
#define CSOUNDWAVE_H_

#include "CRay.h"

class CSoundWave : public CRay
{
public:
	CSoundWave(CMap *p_map, Uint32 x, Uint32 y,
               direction_t dir, object_t byType=OBJ_NONE, size_t byID=0);
	void process();
    
private:
	int animframe, animtimer;
	int offscreentime;
};

#endif /* CSOUNDWAVE_H_ */
