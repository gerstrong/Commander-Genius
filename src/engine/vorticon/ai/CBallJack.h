/*
 * CBallJack.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CBALLJACK_H_
#define CBALLJACK_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include <vector>

class CBallJack : public CObject
{
public:
	CBallJack(CMap *pmap, Uint32 x, Uint32 y,
			std::vector<CPlayer> &PlayerVect);
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

	std::vector<CPlayer> &m_Player;

	direction_t zapd;	   			// x,y, and direction of last shot at time of impact
};


#endif /* CBALLJACK_H_ */
