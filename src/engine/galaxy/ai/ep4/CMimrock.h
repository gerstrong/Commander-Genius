/*
 * CMimrock.h
 *
 *  Created on: 18.09.2011
 *      Author: gerstrong
 */

#ifndef CMIMROCK_H_
#define CMIMROCK_H_

#include "engine/galaxy/ai/CStunnable.h"

namespace galaxy {

class CMimrock: public CStunnable
{
public:

	CMimrock(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	bool isNearby(CSpriteObject &theObject);
	void process();
	void processSit();
	void processWalk();
	void processJump();
	void processBounce();
	void processStunned();
};

} /* namespace galaxy */
#endif /* CMIMROCK_H_ */
