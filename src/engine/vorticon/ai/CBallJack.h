/*
 * CBallJack.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CBALLJACK_H_
#define CBALLJACK_H_

#include "../../../common/CObject.h"

class CBallJack : public CObject
{
public:
	CBallJack();
	void process();

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
};


#endif /* CBALLJACK_H_ */
