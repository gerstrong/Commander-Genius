/*
 * CVortiNinja.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTININJA_H_
#define CVORTININJA_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "../../../common/CPlayer.h"

// Ninja AI (the black, bear-like karate-kicking creature in ep3)
enum ninja_actions{
	NINJA_STAND, NINJA_KICK, NINJA_DYING
};

class CVortiNinja : public CVorticonSpriteObject
{
public:
	CVortiNinja(CMap *p_map, Uint32 x, Uint32 y, std::vector<CPlayer> &Player);
	void init();
	void process();
private:
	ninja_actions state;
	char dir;

	int animframe, animtimer;
	unsigned int timetillkick;

	int dietimer;
	bool longjump;
	std::vector<CPlayer> &m_Player;
};

#endif /* CVORTININJA_H_ */
