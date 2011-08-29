/*
 * CSkypest.h
 *
 *  Created on: 21.08.2011
 *      Author: gerstrong
 *  The Skypest is a fly like creature which can only be killed by squashing it.
 */

#ifndef CSKYPEST_H_
#define CSKYPEST_H_

#include "common/CObject.h"

namespace galaxy {

class CSkypest : public CObject {
public:
	CSkypest(CMap *pmap, Uint32 x, Uint32 y);

	void process();
	void getTouchedBy(CObject &theObject);
	bool isNearby(CObject &theObject);

private:

	void processOnFloor();
	void processFly();

	void (CSkypest::*mp_processState)();
};

} /* namespace galaxy */
#endif /* CSKYPEST_H_ */
