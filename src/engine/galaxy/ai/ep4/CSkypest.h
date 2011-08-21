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
};

} /* namespace galaxy */
#endif /* CSKYPEST_H_ */
