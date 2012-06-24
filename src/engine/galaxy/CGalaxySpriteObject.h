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
public:
	CGalaxySpriteObject(CMap *pmap, Uint32 x, Uint32 y);

	void setupGalaxyObjectOnMap(const size_t ActionBaseOffset = 0x0,
								const size_t ActionNumber = 0x0 );

	void processFalling();

	int checkSolidU(int x1, int x2, int y1, const bool push_mode=false );
	int checkSolidD(int x1, int x2, int y2, const bool push_mode=false );

	bool getActionNumber(int16_t ActionNumber);
	bool getActionStatus(int16_t ActionNumber);
	int16_t getActionNumber();
	void setActionForce(size_t ActionNumber);
	void setAction(size_t ActionNumber);
	void setActionSprite();
	void processActionRoutine();

	// This is used for action format only
	uint16_t m_ActionTicker;
	uint16_t m_ActionNumber;
	size_t m_ActionBaseOffset;
	bool m_jumped;
	bool m_climbing;
	bool m_jumpdown;
};

#endif /* CGALAXYSPRITEOBJECT_H_ */
