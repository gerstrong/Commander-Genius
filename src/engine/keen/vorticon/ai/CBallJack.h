/*
 * CBallJack.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CBALLJACK_H_
#define CBALLJACK_H_

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"
#include <vector>

class CBallJack : public CVorticonSpriteObject
{
public:
	CBallJack(std::shared_ptr<CMap> pMap, Uint32 x, Uint32 y, object_t type);
    void getTouchedBy(CSpriteObject &theObject);
	void process();

	void getShotByRay(object_t &obj_type);

private:
	enum {
	DUPRIGHT,
	DUPLEFT,
	DUP,
	DDOWN,
	DDOWNRIGHT,
	DDOWNLEFT,
	DLEFT,
	DRIGHT
	}m_Direction;

	int animframe, animtimer;
	int speed;

	direction_t zapd;	   			// x,y, and direction of last shot at time of impact
};


#endif /* CBALLJACK_H_ */
