/*
 * CDevilSprite.h
 *
 *  Created on: 30.07.2011
 *      Author: gerstrong
 */

#ifndef CDEVILSPRITE_H_
#define CDEVILSPRITE_H_

#include "common/CObject.h"

namespace galaxy {

class CDevilSprite: public CObject {
public:
	CDevilSprite(CMap *pmap, Uint32 x, Uint32 y);

	void getTouchedBy(CObject &theObject);
	bool isNearby(CObject &theObject);

	void process();

private:
	void processMove();
	void processLook();
	void processShoot();

	void (CDevilSprite::*mp_processState)();

	int m_timer;
};

} /* namespace galaxy */
#endif /* CDEVILSPRITE_H_ */
