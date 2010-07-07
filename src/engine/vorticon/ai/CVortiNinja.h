/*
 * CVortiNinja.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTININJA_H_
#define CVORTININJA_H_

#include "../../../common/CObject.h"

class CVortiNinja : CObject
{
public:
	CVortiNinja(CMap *p_map, Uint32 x, Uint32 y);
	void process();
};

#endif /* CVORTININJA_H_ */
