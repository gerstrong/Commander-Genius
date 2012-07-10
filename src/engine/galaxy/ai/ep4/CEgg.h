/*
 * CEgg.h
 *
 *  Created on: 10.07.2012
 *      Author: gerstong
 *
 *  It processes the egg just laying there
 */

#ifndef CEGG_H_
#define CEGG_H_

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy
{

class CEgg : public CGalaxySpriteObject
{
public:
	CEgg(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	void hatch();


	// When something touches the bouncing mushroom
	void getTouchedBy(CSpriteObject &theObject);

	// Happens every LPS. Here the Bouncing AI is handled
	void process();


};

} /* namespace galaxy */
#endif /* CEGG_H_ */
