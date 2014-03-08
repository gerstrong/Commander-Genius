/*
 * CWormmouth.h
 *
 *  Created on: 16.07.2012
 *      Author: gerstong
 */

#ifndef CWORMMOUTH_H_
#define CWORMMOUTH_H_

#include "../../common/ai/CStunnable.h"

namespace galaxy {

class CWormmouth : public CStunnable
{
public:
	CWormmouth(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	bool isNearby(CSpriteObject &theObject);
	void getTouchedBy(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode );

	void process();

	void processMoving();
	void processLooking();
	void processEating();

private:
	bool mTurnAround;
	int mTimer;
};

} /* namespace galaxy */
#endif /* CWORMMOUTH_H_ */
