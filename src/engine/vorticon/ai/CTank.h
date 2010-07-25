/*
 * tankep2.h
 *
 *  Created on: 25.06.2010
 *      Author: gerstrong
 */

#ifndef TANKEP2_H_
#define TANKEP2_H_

#include "../../../common/CObject.h"
#include "../../../common/CPlayer.h"

#define TANK_SAME_LEVEL_TIME   25

#define TANK_SAME_LEVEL_TIME_FAST    50
#define TANK_REPEAT_FIRE_TIME_FAST   300

// when this probability is satisfied, there is 50% probability
// of a look, 50% probability of a fire.

#define TANK_LOOK_ANIM_TIME     4
#define TANK_PREPAREFIRE_TIME   30
#define TANK_PREPAREFIRE_TIME_FAST   22
#define TANK_WAITAFTER_FIRE		14

// frames
#define TANK_WALK_LEFT_FRAME        102
#define TANK_WALK_RIGHT_FRAME       98
#define TANK_LOOK_FRAME             106

//reference to ../misc.cpp
unsigned int rnd(void);

#define TANK_REPEAT_FIRE_TIME  800

// when this probability is satisfied, there is 50% probability
// of a look, 50% probability of a fire.
#define TANK_LOOKFIRE_PROB    500

#define TANK_MAXTRAVELDIST    200
#define TANK_MINTRAVELDIST    20

#define TANK_WALK_SPEED         28
#define TANK_WALK_ANIM_TIME     6
#define TANK_LOOK_ANIM_TIME     4
#define TANK_LOOK_TOTALTIME     25

#define TANKPUSHAMOUNT        16

#define TANK_FIRE_PAUSE_TIME		25

#define Sprite g_pGfxEngine->Sprite

// this same struct is used for both ep1 and ep2 "tanks", although
// they have seperate ai modules
class CTank : public CObject
{
public:
	CTank(CMap *p_map, Uint32 x, Uint32 y,
			std::vector<CPlayer>& Player, std::vector<CObject*>& Object,
			object_t objtype=OBJ_TANK);
	virtual void process();

	bool CanMoveLeft();
	bool CanMoveRight();
	void getTouchedBy(CObject &theObject);

protected:
	// Tank Robot
	enum{ TANK_WALK,TANK_TURN,TANK_WAIT,TANK_WAIT_LOOK,TANK_FIRE,TANK_LOOK
	} state;

	unsigned int timer,animtimer;
	unsigned char frame;
	unsigned int dist_to_travel;

	unsigned char movedir;

	unsigned int ponsameleveltime;
	unsigned char alreadyfiredcauseonsamelevel;
	unsigned char fireafterlook;

	bool detectedPlayer;         		  // true if player on same level
	unsigned char detectedPlayerIndex;    // index of player that was detected

	// for tank2
	unsigned int timetillnextshot;
	unsigned int firetimes;
	unsigned int timetillcanfire;
	unsigned int timetillcanfirecauseonsamelevel;
	unsigned int turnaroundtimer;
	int pausetime;
	std::vector<CPlayer>& m_Player;
	std::vector<CObject*>& m_Object;
	bool hardmode;
};

#endif /* TANKEP2_H_ */
