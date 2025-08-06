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

#include "../CVorticonSpriteObject.h"

class CRope : public CVorticonSpriteObject
{
public:
	CRope(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y);
	void process();
	void getShotByRay(object_t &obj_type);
	void rope_movestone();
	void getTouchedBy(CSpriteObject &theObject);

private:
	ropestates state;
	int droptimer;
	int stoneX, stoneY;
	int bgtile;
	int falldist;
};

#endif /* CROPE_H_ */
