/*
 * icecannon.h
 *
 *  Created on: 06.08.2009
 *      Author: gerstrong
 */

#ifndef ICECANNON_H_
#define ICECANNON_H_

#include "../../../common/CObject.h"

void delete_object(int o);

unsigned int rnd(void);

//static void smash(int o);

class CIceCannon : public CObject
{
public:
	CIceCannon(CMap *p_map);
	void process();
private:

};

#endif /* ICECANNON_H_ */
