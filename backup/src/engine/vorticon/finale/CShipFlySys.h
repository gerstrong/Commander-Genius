/*
 * CShipFlySys.h
 *
 *  Created on: 09.11.2009
 *      Author: gerstrong
 */

#ifndef CSHIPFLYSYS_H_
#define CSHIPFLYSYS_H_

#include "../../../keen.h"
#include "../../../common/CPlayer.h"

#define CMD_MOVE                0
#define CMD_WAIT                1
#define CMD_SPAWNSPR            2
#define CMD_REMOVESPR           3
#define CMD_FADEOUT             4
#define CMD_ENDOFQUEUE          5
#define CMD_ENABLESCROLLING     6
#define CMD_DISABLESCROLLING    7

#define SPR_SHIP_RIGHT  115
#define SPR_SHIP_LEFT   116

const int SPR_VORTICON_MOTHERSHIP = 72;
const int SPR_SHIP_RIGHT_EP2 = 132;
const int SPR_SHIP_LEFT_EP2 = 133;

#define SPR_EXCLAMATION 117
#define SPR_QUESTION    118
#define SHIPSPD         16

// start x,y map scroll position for eseq1_ShipFlys()
#define SHIPFLY_X       32
#define SHIPFLY_Y       0

typedef struct stShipQueue
{
	int cmd;
	int time;
	int flag1;
} stShipQueue;

class CShipFlySys {
public:
	CShipFlySys(CPlayer &Player, CMap *p_Map, int ship_rightsprite, int ship_leftsprite);
	void addShipQueue(int cmd, int time, int flag1);
	bool EndOfQueue() { return m_finished; }
	void process();
	virtual ~CShipFlySys();

	int m_ShipQueuePtr;

private:
	stShipQueue m_shipqueue[32];
	bool m_finished;
	bool m_scrollingon;
	CPlayer &m_player;
	CObject *mp_mark;
	CMap *mp_Map;
	int m_playsprite_left;
	int m_playsprite_right;
};

#endif /* CSHIPFLYSYS_H_ */
