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

	unsigned int m_index;        	// Like an ID for some objects that need this implementation
	object_t m_type;        		// yorp, vorticon, etc.

	unsigned char touchPlayer;      // 1=hit detection with player
	unsigned char touchedBy;        // which player was hit

};

#endif /* CVORTICONSPRITEOBJECT_H_ */
