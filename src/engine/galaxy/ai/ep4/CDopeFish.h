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

	void getTouchedBy(CObject &theObject);


	// Normal process-cycle
	void process();

	// These function are the action that the Dopefish does
	void processSwim();
	void processEat();
	void processBurp();

private:
	void (CDopeFish::*mp_processState)();
};

} /* namespace galaxy */
#endif /* CDOPEFISH_H_ */
