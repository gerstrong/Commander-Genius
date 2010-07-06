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
	CRisingPoints(CMap *p_map);
	void process();
private:
};

#endif /* CRISINGPOINTS_H_ */
