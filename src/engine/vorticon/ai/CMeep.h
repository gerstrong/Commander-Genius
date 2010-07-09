/*
 * CMeep.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CMEEP_H_
#define CMEEP_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"

class CMeep : public CObject
{
public:
	CMeep(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player,
			std::vector<CObject*>& Object);
	void process();
private:
	char state;
	char dir;

	int animframe, animtimer;
	int timer;

	std::vector<CPlayer>& m_Player;
	std::vector<CObject*>& m_Object;
};

#endif /* CMEEP_H_ */
