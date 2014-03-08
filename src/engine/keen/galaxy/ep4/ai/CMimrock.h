/*
 * CMimrock.h
 *
 *  Created on: 18.09.2011
 *      Author: gerstrong
 */

#ifndef CMIMROCK_H_
#define CMIMROCK_H_

#include "../../common/ai/CStunnable.h"

namespace galaxy {

class CMimrock: public CStunnable
{
public:

	CMimrock(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

	void getTouchedBy(CSpriteObject &theObject);
	bool isNearby(CSpriteObject &theObject);
	
	void process();
	void processSit();
	void processWalk();
	void processJump();
	void processBounce();
	
private:
    int mTimer;
};

} /* namespace galaxy */
#endif /* CMIMROCK_H_ */
