/*
 *  Created on: 25.06.2010
 *      Author: gerstrong
 */

#ifndef TANKEP1_H_
#define TANKEP1_H_

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"

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

#define Sprite gGraphics.Sprite

// this same struct is used for both ep1 and ep2 "tanks", although
// they have seperate ai modules
class CTank : public CVorticonSpriteObject
{
public:
	CTank(std::shared_ptr<CMap> p_map, Uint32 x, Uint32 y,object_t objtype=OBJ_TANK);
	virtual void process();

	void getTouchedBy(CSpriteObject &theObject);
	bool isNearby(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode=false );

protected:
	// Tank Robot
	enum{ TANK_WALK,TANK_TURN,TANK_WAIT,TANK_WAIT_LOOK,TANK_FIRE,TANK_LOOK
	} state;

	unsigned int timer,animtimer;
	unsigned char frame;
	unsigned int dist_to_travel;

	direction_t movedir;

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
	bool hardmode;
	
	int shotHeight;
};

#endif /* TANKEP1_H_ */
