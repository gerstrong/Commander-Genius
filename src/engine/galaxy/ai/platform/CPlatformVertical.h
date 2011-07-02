/*
 * CPlatformVertical.h
 *
 *  Created on: 06.04.2011
 *      Author: gerstrong
 *
 *  This is the code for the vertical platforms in galaxy
 */

#ifndef CPLATFORMVERTICAL_H_
#define CPLATFORMVERTICAL_H_

#include "CPlatform.h"

namespace galaxy {

class CPlatformVertical : public CPlatform {
public:
	CPlatformVertical(CMap *pmap, const Uint32 x, const Uint32 y);

	void process();
};


}

#endif /* CPLATFORMVERTICAL_H_ */

