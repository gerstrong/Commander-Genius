/*
 * CPlayerBase.h
 *
 *  Created on: 19.06.2011
 *      Author: gerstrong
 *
 *  This is the base class for the Player. This class is very new and I thought of it for
 *  the following reason. We have Keen on the Map in level and when diving.
 *  Those AIs are different of course, but still many things in common like dying
 *  (See the puzzle Keen 5 mod versions by Mink) and collecting items.
 *  It will not only reduce the code of some repeating routines, but also
 *  provide better overview over the whole thing.
 */

#ifndef CPLAYERBASE_H_
#define CPLAYERBASE_H_

#include "common/CObject.h"

class CPlayerBase : CObject {
public:
	CPlayerBase();
	virtual ~CPlayerBase();
};

#endif /* CPLAYERBASE_H_ */
