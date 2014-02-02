/*
 * CBubbles.h
 *
 *  Created on: 29.07.2011
 *      Author: gerstrong
 *
 *  This is the Big Bubble spawn by the Dopefish when he burps
 */

#ifndef CBUBBLES_H_
#define CBUBBLES_H_

#include "../../common/CGalaxySpriteObject.h"

namespace galaxy {

class CBubbles: public CGalaxySpriteObject
{
public:
	CBubbles(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const bool bigbubble);

	// Main process cycle of the Bubble
	void process();
};

} /* namespace galaxy */
#endif /* CBUBBLES_H_ */
