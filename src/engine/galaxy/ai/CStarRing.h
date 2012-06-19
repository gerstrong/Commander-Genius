/*
 * CStarRing.h
 *
 *  Created on: 25.06.2011
 *      Author: gerstrong
 *
 *  This object is spawned whenever an enemy gets stunned. it justs represents the stars above
 *  his head. Note: Not all Enemies use this, but most of them
 */

#ifndef CSTARRING_H_
#define CSTARRING_H_

#include "common/CSpriteObject.h"

namespace galaxy
{

const unsigned int STARRING_SPRITE = 40;

class CStarRing : public CSpriteObject {
public:
	CStarRing(CMap *pmap, Uint32 x, Uint32 y, object_t type);

private:
	void process();

	unsigned char m_animation_timer;
};

};

#endif /* CSTARRING_H_ */
