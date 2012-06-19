/*
 * CGuardRobot.h
 *
 *  Created on: 25.07.2010
 *      Author: gerstrong
 */

#ifndef CGUARDROBOT_H_
#define CGUARDROBOT_H_

#include "common/CSpriteObject.h"
#include "common/CPlayer.h"

//reference to ../misc.cpp
unsigned int rnd(void);

class CGuardRobot : public CSpriteObject
{
public:
	CGuardRobot(CMap *p_map, Uint32 x, Uint32 y,
			 std::vector<CSpriteObject*>& Object);
	void process();
	void guard_fire();
	void getTouchedBy(CSpriteObject &theObject);

private:

	std::vector<CSpriteObject*>& m_ObjectVect;

	enum{ WALK, TURN, WAIT, WAIT_LOOK, FIRE, LOOK } state;

	unsigned int timer,animtimer;
	unsigned char frame;
	unsigned int dist_to_travel;

	unsigned char movedir;

	unsigned char fireafterlook;

	unsigned int timetillnextshot;
	unsigned int firetimes;
	unsigned int timetillcanfire;
	unsigned int timetillcanfirecauseonsamelevel;
	unsigned int turnaroundtimer;
	int pausetime;
	bool hardmode;
};

#endif /* CGUARDROBOT_H_ */
