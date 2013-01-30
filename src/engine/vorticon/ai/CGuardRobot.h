/*
 * CGuardRobot.h
 *
 *  Created on: 25.07.2010
 *      Author: gerstrong
 */

#ifndef CGUARDROBOT_H_
#define CGUARDROBOT_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CPlayer.h"

//reference to ../misc.cpp
unsigned int rnd(void);

class CGuardRobot : public CVorticonSpriteObject
{
public:
	CGuardRobot(CMap *p_map, Uint32 x, Uint32 y);
	void process();
	void guard_fire();
	void getTouchedBy(CSpriteObject &theObject);
    
private:
    
	enum{ WALK, TURN, WAIT, WAIT_LOOK, FIRE, LOOK } state;
    
	unsigned int timer,animtimer;
	unsigned char frame;
	unsigned int dist_to_travel;
    
	char movedir;
    
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
