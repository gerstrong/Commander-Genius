/*
 * CMeep.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CMEEP_H_
#define CMEEP_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "../../../common/CPlayer.h"

class CMeep : public CVorticonSpriteObject
{
public:
	CMeep(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player,
			std::vector<CVorticonSpriteObject*>& Object);
	void process();
private:
	char state;
	char dir;

	int animframe, animtimer;
	int timer;

	std::vector<CPlayer>& m_Player;
	std::vector<CVorticonSpriteObject*>& m_Object;
};

#endif /* CMEEP_H_ */
