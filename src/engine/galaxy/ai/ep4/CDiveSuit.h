/*
 * CDiveSuit.h
 *
 *  Created on: 25.06.2011
 *      Author: gerstrong
 */

#ifndef CDIVESUIT_H_
#define CDIVESUIT_H_

#include "engine/galaxy/CGalaxySpriteObject.h"

namespace galaxy {

class CDiveSuit: public CGalaxySpriteObject
{
public:
	CDiveSuit(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y);

private:
	void process();
	void getTouchedBy(CSpriteObject &theObject);

	bool taken;
};

} /* namespace galaxy */
#endif /* CDIVESUIT_H_ */
