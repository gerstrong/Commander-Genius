/*
 * CMeep.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CMEEP_H_
#define CMEEP_H_

#include "../../../common/CObject.h"

class CMeep : CObject
{
public:
	CMeep(CMap *p_map, Uint32 x, Uint32 y);
	void process();
private:
	char state;
	char dir;

	int animframe, animtimer;
	int timer;
};

#endif /* CMEEP_H_ */
