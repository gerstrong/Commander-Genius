/*
 * CVortiMom.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CVORTIMOM_H_
#define CVORTIMOM_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"
#include <vector>

enum vortimom_actions{
MOTHER_WALK, MOTHER_SPIT,
MOTHER_HURT
};

// Vorticon Mother AI (ep3)

#define MOTHER_WALK_ANIM_RATE     3
#define MOTHER_WALK_SPD           15

#define MOTHER_SPIT_PROB          4
#define MOTHER_SPIT_PROB_HARD     6
#define MOTHER_SPIT_SHOW_TIME     25

#define MOTHER_HP      5

#define MOTHER_HURT_SHOW_TIME    25

#define SNDWAVE_LEFT_FRAME    128

#define MOTHER_WALK_LEFT_FRAME	 85
#define MOTHER_WALK_RIGHT_FRAME	 87
#define MOTHER_SPIT_LEFT_FRAME	 89
#define MOTHER_SPIT_RIGHT_FRAME	 90
#define MOTHER_HURT_FRAME        91
#define MOTHER_DEAD_FRAME	 92

class CVortiMom : public CObject
{
public:
	CVortiMom(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player,
			std::vector<CObject*>& Object);
	void process();
	void getShotByRay();

private:
	vortimom_actions state;
	direction_t dir;

	int animframe, animtimer;
	int timer;


	std::vector<CPlayer>& m_Player;
	std::vector<CObject*>& m_Object;
};

#endif /* CVORTIMOM_H_ */
