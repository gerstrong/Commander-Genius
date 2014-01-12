/*
 * CMastersSpark.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Masters Spark in Galaxy
 */
 
#ifndef CMASTERSSPARK_H_
#define CMASTERSSPARK_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CMastersSpark : public CStunnable
{
public:
	CMastersSpark(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * Action functions
	 */
	void processWalking();	

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
};

}

#endif /* CMASTERSSPARK_H_ */
