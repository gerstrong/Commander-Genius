/*
 * CSpark.h
 *
 *  Created on: 09.07.2010
 *      Author: gerstrong
 */

#ifndef CSPARK_H_
#define CSPARK_H_

#include "../CVorticonSpriteObject.h"
#include "engine/core/spritedefines.h"

class CSpark : public CVorticonSpriteObject {
public:
	CSpark(CMap *pmap, Uint32 x, Uint32 y);
	void process();

private:
	enum spark_actions{
	SPARK_ANIMATE, SPARK_BLOWUP1, SPARK_BLOWUP2, SPARK_BLOWUP3
	}state;

	unsigned int timer;
	unsigned int frame;
	int blowx,blowy;
};

#endif /* CSPARK_H_ */
