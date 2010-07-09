/*
 * CRisingPoints.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CRISINGPOINTS_H_
#define CRISINGPOINTS_H_

#include "../../../common/CObject.h"

class CRisingPoints : public CObject
{
public:
	CRisingPoints(CMap *p_map, Uint32 x, Uint32 y);
	void process();
private:
	size_t offscreentime;
};

#endif /* CRISINGPOINTS_H_ */
