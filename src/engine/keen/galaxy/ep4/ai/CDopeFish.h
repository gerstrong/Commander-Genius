/*
 * CDopeFish.h
 *
 *  Created on: 26.07.2011
 *      Author: gerstrong
 */

#ifndef CDOPEFISH_H_
#define CDOPEFISH_H_

#include "../../common/CGalaxySpriteObject.h"

namespace galaxy {

class CDopeFish : public CGalaxySpriteObject
{
public:
    CDopeFish(std::shared_ptr<CMap> pMap, const Uint16 foeID, Uint32 x, Uint32 y, const int sprVar);

	// When Dopefish gets touched by something
	void getTouchedBy(CSpriteObject &theObject);

	// Normal process-cycle
	void process();

	// Overloaded functions
	bool isNearby(CSpriteObject &theObject);

private:
	// Basic movement of the Dopefish
	void moveDope(const int speed);

	// These function are the action that the Dopefish does
	void processSwim();
	void processEat();
	void processBurp();

	void (CDopeFish::*mp_processState)();

	int m_eatTimer;
	bool m_burped;
};

} /* namespace galaxy */
#endif /* CDOPEFISH_H_ */
