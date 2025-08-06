/*
 * CButler.h
 *
 *  Created on: 05.07.2010
 *      Author: gerstrong
 */

#ifndef CBUTLER_H_
#define CBUTLER_H_

#include "../CVorticonSpriteObject.h"
#include "CPlayer.h"
#include <vector>

#define BUTLER_WALK_SPEED        28
#define BUTLER_WALK_SPEED_FAST   38
#define BUTLER_WALK_ANIM_TIME    6
#define BUTLER_WALK_ANIM_TIME_FAST    1
#define BUTLER_TURN_TIME         10

#define BUTLERPUSHAMOUNT         60
#define BUTLERPUSHAMOUNTFAST     30

// distance in pixels butler should look ahead to avoid falling
// off an edge
#define BUTLER_LOOK_AHEAD_DIST   1

// frames
#define BUTLER_WALK_LEFT_FRAME   92
#define BUTLER_WALK_RIGHT_FRAME  88
#define BUTLER_TURNLEFT_FRAME    96
#define BUTLER_TURNRIGHT_FRAME   97


class CButler : public CVorticonSpriteObject
{
public:
	CButler(std::shared_ptr<CMap> pMap, Uint32 x, Uint32 y);
	void process();
	void getTouchedBy(CSpriteObject &theObject);

	int checkSolidD( int x1, int x2, int y2, const bool push_mode=false );

private:
	// AI for "butler" robot (ep1)
	enum {
		BUTLER_TURN, BUTLER_WALK, BUTLER_FALL
	} state;
	char timer,animtimer;
	char frame;
};

#endif /* CBUTLER_H_ */
