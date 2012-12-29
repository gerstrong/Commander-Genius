/*
 * CFuse.h
 *
 *  Created on: 29 Dez 2012
 *      Author: Gerstrong
 *
 *  This handles the Fuses in Keen 5
 */
 
#ifndef CFUSE_H_
#define CFUSE_H_

#include <vector>
#include "engine/galaxy/common/CGalaxySpriteObject.h"

namespace galaxy {


class CFuse : public CGalaxySpriteObject
{
public:
	CFuse(CMap *pmap, const Uint16 foeID, const Uint32 x, const Uint32 y);

	void process();

	/**
	 * What happens if the slug gets touched by another object
	 */
	void getTouchedBy(CSpriteObject &theObject);
	
private:
	int mTimer;
};

}

#endif /* CFUSE_H_ */
