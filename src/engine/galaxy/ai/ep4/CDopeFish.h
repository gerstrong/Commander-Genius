/*
 * CDopeFish.h
 *
 *  Created on: 26.07.2011
 *      Author: gerstrong
 */

#ifndef CDOPEFISH_H_
#define CDOPEFISH_H_

#include "common/CObject.h"

namespace galaxy {

class CDopeFish : public CObject {
public:
	CDopeFish(CMap *pmap, Uint32 x, Uint32 y);

	// When Dopefish gets touched by something
	void getTouchedBy(CObject &theObject);

	// Normal process-cycle
	void process();

	// Overloaded functions
	bool isNearby(CObject &theObject);

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
