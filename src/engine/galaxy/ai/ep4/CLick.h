/*
 * CLick.h
 *
 *  Created on: 04.08.2011
 *      Author: gerstrong
 *
 *  Lick is an enemy of Ep4 which looks like a smaller blue ball which
 *  breathes fire to kill keen. It also bounces around not as high as bounder
 *  and can kill, as soon as you touch his fire
 */

#ifndef CLICK_H_
#define CLICK_H_

#include "engine/galaxy/ai/CStunnable.h"

namespace galaxy {

class CLick : public CStunnable
{
public:
	CLick(CMap *pmap, Uint32 x, Uint32 y);

	void process();

	void getTouchedBy(CSpriteObject &theObject);
	bool isNearby(CSpriteObject &theObject);

private:
	void processHop();
	void processLand();
	void processBreathe();
	void processStunned();

	int m_timer;
};

} /* namespace galaxy */
#endif /* CLICK_H_ */
