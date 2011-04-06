/*
 * CPlatformVertical.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 */

#ifndef CPLATFORMVERTICAL_H_
#define CPLATFORMVERTICAL_H_

#include "CPlatform.h"

namespace galaxy {

class CPlatformVertical {
public:
	CPlatformVertical(CMap *pmap, const Uint32 x, const Uint32 y,
			std::vector<CObject*>& ObjectPtrs);

	void process();
};


}

#endif /* CPLATFORMVERTICAL_H_ */

