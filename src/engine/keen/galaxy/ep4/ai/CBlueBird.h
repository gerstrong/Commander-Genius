/*
 * CBlueBird.h
 *
 *  Created on: 11.07.2012
 *      Author: gerstong
 */

#ifndef CBLUEBIRD_H_
#define CBLUEBIRD_H_

#include "engine/galaxy/common/CGalaxySpriteObject.h"

namespace galaxy {

class CBlueBird: public CGalaxySpriteObject
{
public:
	CBlueBird(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	void setActionForce(const size_t ActionNumber);

	bool isNearby(CSpriteObject &theObject);

	void getTouchedBy(CSpriteObject &theObject);
	int checkSolidD( int x1, int x2, int y2, const bool push_mode );
	void process();

private:

	void processHatched();
	void processWalking();
	void processFlying();
	void processStunned();


	void (CBlueBird::*mpProcessState)();
	std::map< size_t, void (CBlueBird::*)() > mActionMap;
	int mTimer;
};

} /* namespace galaxy */
#endif /* CBLUEBIRD_H_ */
