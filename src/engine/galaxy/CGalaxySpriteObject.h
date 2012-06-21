/*
 * CGalaxySpriteObject.h
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#ifndef CGALAXYSPRITEOBJECT_H_
#define CGALAXYSPRITEOBJECT_H_

#include "common/CSpriteObject.h"

class CGalaxySpriteObject: public CSpriteObject
{
	CGalaxySpriteObject(CMap *pmap, Uint32 x, Uint32 y, object_t type);
};

#endif /* CGALAXYSPRITEOBJECT_H_ */
