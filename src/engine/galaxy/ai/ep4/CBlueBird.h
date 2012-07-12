/*
 * CBlueBird.h
 *
 *  Created on: 11.07.2012
 *      Author: gerstong
 */

#ifndef CBLUEBIRD_H_
#define CBLUEBIRD_H_

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy {

class CBlueBird: public CGalaxySpriteObject
{
public:
	CBlueBird(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	void setActionForce(const size_t ActionNumber);

	bool isNearby(CSpriteObject &theObject);
	void processHatched();
	void processWalking();
	void processFlying();
	void processStunned();

	void getTouchedBy(CSpriteObject &theObject);
	void process();

private:
	void (CBlueBird::*mpProcessState)();
	std::map< size_t, void (CBlueBird::*)() > mActionMap;
	int mTimer;
};

} /* namespace galaxy */
#endif /* CBLUEBIRD_H_ */
