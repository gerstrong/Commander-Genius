/*
 * CWaterMine.h
 *
 *  Created on: 30.07.2011
 *      Author: gerstrong
 *
 *  The Watermines are normally in the well of wishes
 */

#ifndef CWATERMINE_H_
#define CWATERMINE_H_

#include "engine/galaxy/common/CGalaxySpriteObject.h"

namespace galaxy {

class CWaterMine: public CGalaxySpriteObject
{
public:
	CWaterMine(CMap *pmap, const Uint16 foeID, Uint32 x, Uint32 y, const direction_t xDir, bool vertical);

	void process();

	// What happens when some object touches another one
	void getTouchedBy(CSpriteObject &theObject);

private:

	void processMove();
	void processExplode();

	void (CWaterMine::*mp_processState)();
};

} /* namespace galaxy */
#endif /* CWATERMINE_H_ */
