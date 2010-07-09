/*
 * CRope.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CROPE_H_
#define CROPE_H_

enum ropestates{
ROPE_IDLE, ROPE_DROPSTONE
};

#include "../../../common/CObject.h"

class CRope : public CObject
{
public:
	CRope(CMap *p_map, Uint32 x, Uint32 y);
	void process();
	void getShotByRay();
	void rope_movestone();
	void getTouchedBy(CObject &theObject);

private:
	ropestates state;
	int droptimer;
	int stoneX, stoneY;
	int bgtile;
};

#endif /* CROPE_H_ */
