/*
 * CGuardRobot.h
 *
 *  Created on: 25.07.2010
 *      Author: gerstrong
 */

#ifndef CGUARDROBOT_H_
#define CGUARDROBOT_H_

#include "common/CObject.h"
#include "common/CPlayer.h"

//reference to ../misc.cpp
unsigned int rnd(void);

class CGuardRobot : public CObject
{
public:
	CGuardRobot(CMap *p_map, Uint32 x, Uint32 y,
			 std::vector<CObject*>& Object);
	void process();
	void tank2_fire();

private:

	std::vector<CObject*>& m_ObjectVect;

	enum{ TANK_WALK,TANK_TURN,TANK_WAIT,TANK_WAIT_LOOK,TANK_FIRE,TANK_LOOK
	} state;

	unsigned int timer,animtimer;
	unsigned char frame;
	unsigned int dist_to_travel;

	unsigned char movedir;

	unsigned int ponsameleveltime;
	unsigned char alreadyfiredcauseonsamelevel;
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
