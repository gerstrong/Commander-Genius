/*
 * CSparky.h
 *
 *  Created on: 26 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Sparkies in Galaxy
 */
 
#ifndef CSPARKY_H_
#define CSPARKY_H_

#include <vector>
#include "engine/galaxy/common/ai/CStunnable.h"

namespace galaxy {


class CSparky : public CStunnable
{
public:
	CSparky(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	/**
	 * When it is moving normally
	 */
	void processMoving();

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
};

}

#endif /* CSPARKY_H_ */
