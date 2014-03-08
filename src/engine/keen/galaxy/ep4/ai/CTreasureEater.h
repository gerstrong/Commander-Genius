/*
 * CTreasureEater.h
 *
 *  Created on: 17.07.2012
 *      Author: gerstong
 */

#ifndef CTREASUREEATER_H_
#define CTREASUREEATER_H_

#include "../../common/ai/CStunnable.h"

namespace galaxy {

class CTreasureEater: public CStunnable
{
public:
	CTreasureEater(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	bool isNearby(CSpriteObject &theObject);
	void getTouchedBy(CSpriteObject &theObject);

	void processLooking();
	void processTeleporting();
	bool lookForNextDestination();
	void processHopping();

	void checkForItem();
	void process();

private:

	int mTimer;
	bool mTeleported;
	bool mStolen;
	VectorD2<int> mDestination;
};

} /* namespace galaxy */
#endif /* CTREASUREEATER_H_ */
