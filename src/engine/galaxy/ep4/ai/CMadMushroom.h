/*
 * CMadMushroom.h
 *
 *  Created on: 12.07.2011
 *      Author: gerstrong
 */

#ifndef CMADMUSHROOM_H_
#define CMADMUSHROOM_H_

#include "engine/galaxy/common/CGalaxySpriteObject.h"

namespace galaxy
{

class CMadMushroom : public CGalaxySpriteObject
{
public:
	CMadMushroom(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	// Checks if some object is nearby. Here the mushroom turns to watch the player
	bool isNearby(CSpriteObject &theObject);

	// When something touches the bouncing mushroom
	void getTouchedBy(CSpriteObject &theObject);

	// Happens every LPS. Here the Bouncing AI is handled
	void process();

private:

	int jumpcounter;
};

} /* namespace galaxy */
#endif /* CMADMUSHROOM_H_ */
