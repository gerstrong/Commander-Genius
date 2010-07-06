/*
 * CFireBall.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CFIREBALL_H_
#define CFIREBALL_H_

#include "CRay.h"

class CFireBall : public CRay
{
	CFireBall(CMap *p_map, Uint32 x, Uint32 y,
			direction_t dir, object_t byType, size_t byID);
	void process();
};


#endif /* CFIREBALL_H_ */
