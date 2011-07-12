/*
 * CMadMushroom.h
 *
 *  Created on: 12.07.2011
 *      Author: gerstrong
 */

#ifndef CMADMUSHROOM_H_
#define CMADMUSHROOM_H_

#include "common/CObject.h"

namespace galaxy
{

class CMadMushroom : public CObject
{
public:
	CMadMushroom(CMap *pmap, Uint32 x, Uint32 y);

	// Checks if some object is nearby. Here the mushroom turns to watch the player
	bool isNearby(CObject &theObject);

	// When something touches the bouncing mushroom
	void getTouchedBy(CObject &theObject);

	// Happens every LPS. Here the Bouncing AI is handled
	void process();

private:

	int jumpcounter;
};

} /* namespace galaxy */
#endif /* CMADMUSHROOM_H_ */
