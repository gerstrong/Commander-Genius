/*
 * CBigBubble.h
 *
 *  Created on: 29.07.2011
 *      Author: gerstrong
 *
 *  This is the Big Bubble spawn by the Dopefish when he burps
 */

#ifndef CBIGBUBBLE_H_
#define CBIGBUBBLE_H_

#include "common/CObject.h"

namespace galaxy {

class CBigBubble: public CObject {
public:
	CBigBubble(CMap *pmap, Uint32 x, Uint32 y);

	// Main process cycle of the Bubble
	void process();
};

} /* namespace galaxy */
#endif /* CBIGBUBBLE_H_ */
