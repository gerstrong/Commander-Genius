/*
 * CDiveSuit.h
 *
 *  Created on: 25.06.2011
 *      Author: gerstrong
 */

#ifndef CDIVESUIT_H_
#define CDIVESUIT_H_

#include "common/CObject.h"

namespace galaxy {

class CDiveSuit: public CObject {
public:
	CDiveSuit(CMap *pmap, Uint32 x, Uint32 y);

private:
	void process();
	void getTouchedBy(CObject &theObject);

	bool taken;
};

} /* namespace galaxy */
#endif /* CDIVESUIT_H_ */
