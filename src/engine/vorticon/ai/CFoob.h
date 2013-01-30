/*
 * CFoob.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 *  AI for the foobs
 *  (yellow "scaredy cat" creatures) (ep3)
 */

#ifndef CFOOB_H_
#define CFOOB_H_

#include "engine/vorticon/CVorticonSpriteObject.h"
#include "common/CPlayer.h"
#include <vector>

#define FOOB_WALK_SPEED      15
#define FOOB_WALK_ANIM_RATE  4

#define FOOB_FLEE_SPEED      77
#define FOOB_FLEE_ANIM_RATE  1

#define FOOB_SPOOK_SHOW_TIME    12

#define FOOB_HARDMODE_BLOCK_TIME	35

#define FOOB_EXPLODE_ANIM_RATE  8

#define FOOB_SPOOK_TIME         40
#define FOOB_RELAX_TIME         400

#define FOOB_WALK_LEFT_FRAME    93
#define FOOB_WALK_RIGHT_FRAME   95
#define FOOB_SPOOK_FRAME        97
#define FOOB_EXPLODE_FRAME      97
#define FOOB_DEAD_FRAME         101

class CFoob : public CVorticonSpriteObject
{
public:
	CFoob(CMap *p_map, Uint32 x, Uint32 y);
	void process();
	bool isNearby(CVorticonSpriteObject &theObject);
	void getTouchedBy(CVorticonSpriteObject &theObject);
    
private:
	enum FOOB_ACTIONS
	{
		FOOB_WALK,
		FOOB_SPOOK,
		FOOB_FLEE,
		FOOB_EXPLODE,
		FOOB_DEAD
	} state;
	direction_t dir;
    
	int animframe, animtimer;
	int OnSameLevelTime;
	int OffOfSameLevelTime;
	int spooktimer;
	int SpookedByWho;
	int blockedtime;
	bool onsamelevel;
};

#endif /* CFOOB_H_ */
