/*
 * CYorp.h
 *
 *  Created on: 04.07.2010
 *      Author: gerstrong
 */

#ifndef CYORP_H_
#define CYORP_H_

#include "../../../common/CPlayer.h"
#include "engine/vorticon/CVorticonSpriteObject.h"
#include "../../../common/CMap.h"

#define OBJ_YORP_DEFSPRITE       50

#define YORP_LOOK_TIME  16   // time each frame of YORP_LOOK is shown
#define YORP_STUN_ANIM_TIME  5
#define YORP_WALK_ANIM_TIME  5
#define YORP_WALK_SPEED      19
#define YORP_WALK_ANIM_TIME_FAST  5
#define YORP_WALK_SPEED_FAST      19

#define YORP_NUM_LOOKS  12      // number of times yorp look frame is changed
#define YORP_STUNTIME   24     // YORP_NUM_LOOKS for stun

// for INCREASE_DIFFICULTY
#define YORP_NUM_LOOKS_FAST  8      // number of times yorp look frame is changed
#define YORP_STUNTIME_FAST   18     // YORP_NUM_LOOKS for stun

#define YORP_LOOK_LEFT  49
#define YORP_STAND      50
#define YORP_LOOK_RIGHT 51
#define YORP_WALK_RIGHT 52
#define YORP_WALK_LEFT  54
#define YORP_STUNFRAME  56

#define YORP_JUMP_PROB      8
#define YORP_JUMP_HEIGHT    -40

#define YORP_DYING_FRAME   58
#define YORP_DEAD_FRAME    59
#define YORP_DIE_TIME      22

#define YORPDIE_START_INERTIA      -40
#define YORPDIE_MAX_INERTIA         480
#define YORPDIE_INERTIA_DECREASE    8

// How much Yorps pushes keen
#define YORP_PUSH_AMT_NO_WALK		60

#define YORP_PUSH_AMT_P_WALK_HARD	100
#define YORP_PUSH_AMT_P_WALK		60

#define YORP_PUSH_AMT_P_STAND_HARD	100
#define YORP_PUSH_AMT_P_STAND		60

class CYorp : public CVorticonSpriteObject
{
public:
	CYorp( CMap *p_map, Uint32 x, Uint32 y );
	void getTouchedBy(CVorticonSpriteObject &theObject);
	bool isNearby(CVorticonSpriteObject &theObject);
	void process();
	
    
private:
	void processLooking();
	void processMoving();
	void processStunned();
	void processDying();
    
	bool m_hardmode;
    
	enum
	{
		YORP_LOOK,     // yorp is "looking around" (waving his eye)
		YORP_MOVE,     // yorp is walking either left or right
		YORP_STUNNED,  // the yorp is stunned
		YORP_DYING    // the yorp is getting fried!
	} state;
    
	unsigned char looktimes,lookposition;
	unsigned char timer, dietimer;
	unsigned char walkframe;
	unsigned int dist_traveled;
    
	direction_t movedir;
};

#endif /* CYORP_H_ */
