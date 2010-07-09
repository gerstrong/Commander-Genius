/*
 * CVortiNinja.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTININJA_H_
#define CVORTININJA_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"

// Ninja AI (the black, bear-like karate-kicking creature in ep3)
enum ninja_actions{
	NINJA_STAND, NINJA_KICK, NINJA_DYING, NINJA_DEAD
};

class CVortiNinja : public CObject
{
public:
	CVortiNinja(CMap *p_map, Uint32 x, Uint32 y, std::vector<CPlayer> &Player);
	void process();
private:
	ninja_actions state;
	char dir;

	int animframe, animtimer;
	unsigned int timetillkick;

	signed int XInertia, YInertia;
	unsigned int XFrictionTimer, YFrictionTimer;
	unsigned int XFrictionRate, YFrictionRate;
	int KickMoveTimer;
	int isdying;
	int dietimer;
	std::vector<CPlayer> &m_Player;
};

#endif /* CVORTININJA_H_ */
