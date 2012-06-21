/*
 * CVorticonSpriteObject.h
 *
 *  Created on: 21.06.2012
 *      Author: gerstrong
 */

#ifndef CVORTICONSPRITEOBJECT_H_
#define CVORTICONSPRITEOBJECT_H_

#include "common/CSpriteObject.h"

class CVorticonSpriteObject : public CSpriteObject
{
public:
	CVorticonSpriteObject(CMap *pmap, Uint32 x, Uint32 y, object_t type);

	void setupObjectType(const int Episode);
	bool checkforScenario();

	bool calcVisibility();

	object_t m_type;        		// yorp, vorticon, etc.

};

#endif /* CVORTICONSPRITEOBJECT_H_ */
