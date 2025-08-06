/*
 * CFoot.h
 *
 *  Created on: Sep 27, 2012
 *      Author: gerstrong
 */

#ifndef CFOOT_H_
#define CFOOT_H_

#include "../../common/CGalaxySpriteObject.h"

namespace galaxy {

class CFoot: public CGalaxySpriteObject
{
public:
	CFoot(std::shared_ptr<CMap> pMap, const Uint16 foeID, const int actionID, const Uint32 x, const Uint32 y);

	void getTouchedBy(CSpriteObject &theObject);

};

} /* namespace galaxy */
#endif /* CFOOT_H_ */
