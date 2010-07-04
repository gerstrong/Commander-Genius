/*
 * CGarg.h
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 */

#ifndef CGARG_H_
#define CGARG_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include <vector>

#define GARG_MINTRAVELDIST          100
#define GARG_LOOK_PROB              100
#define GARG_WALK_SPEED             19
#define GARG_WALK_ANIM_TIME         5
#define GARG_WALK_SPEED_FAST        30
#define GARG_WALK_ANIM_TIME_FAST    3
#define GARG_CHARGE_SPEED           68
#define GARG_CHARGE_ANIM_TIME       7
#define GARG_JUMP_TIME       		160
#define GARG_JUMP_SPEED				96

#define GARG_LOOK_TIME  17
#define GARG_NUM_LOOKS  3

// amount of time keen must be on same level before garg will charge
#define GARG_SAME_LEVEL_TIME     50

#define GARG_STAND         60
#define GARG_WALK_RIGHT    64
#define GARG_WALK_LEFT     66
#define GARG_DYING_FRAME   68
#define GARG_DEAD_FRAME    69

#define GARGDIE_START_INERTIA      -80

// Reference to ../misc.cpp
unsigned int rnd(void);

class CGarg : public CObject
{
public:
	CGarg(std::vector<CPlayer> &PlayerVect, char difficulty,  CMap *p_map);
	void process();
	virtual ~CGarg();

private:
	enum garg_states{
		GARG_LOOK, GARG_MOVE, GARG_CHARGE,
		GARG_JUMP, GARG_DYING, GARG_DEAD
	}state;

	int jumptime;

	unsigned char looktimes,lookframe;
	unsigned char timer, keenonsameleveltimer;
	unsigned char about_to_charge;
	unsigned char walkframe;
	unsigned int dist_traveled;
	signed int gargdie_inertia_y;

	unsigned char movedir;
	unsigned char detectedPlayer, detectedPlayerIndex;

	std::vector<CPlayer> &m_Player;
	bool m_hardmode;
};

#endif /* CGARG_H_ */
