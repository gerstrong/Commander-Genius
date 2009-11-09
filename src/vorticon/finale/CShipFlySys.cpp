/*
 * CShipFlySys.cpp
 *
 *  Created on: 09.11.2009
 *      Author: gerstrong
 */

#include "CShipFlySys.h"

CShipFlySys::CShipFlySys(CPlayer *p_Player, CMap *p_Map)
{
	// TODO Auto-generated constructor stub
	mp_Map = p_Map;
	mp_player = p_Player;
	m_finished = false;
	m_scrollingon = true;
	m_ShipQueuePtr = 0;
}

void CShipFlySys::addShipQueue(int cmd, int time, int flag1)
{
	m_shipqueue[m_ShipQueuePtr].cmd = cmd;
	m_shipqueue[m_ShipQueuePtr].time = time;
	m_shipqueue[m_ShipQueuePtr].flag1 = flag1;
	m_ShipQueuePtr++;
}

void CShipFlySys::process()
{
	int x, y;
	/*std::vector<CObject> &objvect = *(mp_player[0].mp_object);
	CObject
	objvect[MARK_SPR_NUM].type = OBJ_YORP;                // doesn't matter
	objvect[MARK_SPR_NUM].exists = 0;
	objvect[MARK_SPR_NUM].sprite = SPR_QUESTION;*/

	 mp_player[0].playframe = SPR_SHIP_RIGHT;

	 //max_scroll_x = max_scroll_y = 20000;

	 // keep the question or exclamation mark sprite next to the player
	 /*objects[MARK_SPR_NUM].x = mp_player[0].x + (20<<CSF);
	 objects[MARK_SPR_NUM].y = mp_player[0].y - (10<<CSF);
	 objects[MARK_SPR_NUM].onscreen = 1;
	 objects[MARK_SPR_NUM].scrx = (objects[MARK_SPR_NUM].x>>CSF)-scroll_x;
	 objects[MARK_SPR_NUM].scry = (objects[MARK_SPR_NUM].y>>CSF)-scroll_y;*/

	 // execute the current command in the queue
	 switch(m_shipqueue[m_ShipQueuePtr].cmd)
	 {
	 case CMD_MOVE:
		 switch(m_shipqueue[m_ShipQueuePtr].flag1)
		 {
		 case DUP:
			 mp_player[0].y-=SHIPSPD;
			 mp_player[0].playframe = SPR_SHIP_RIGHT;
			 break;
		 case DDOWN:
			 mp_player[0].y+=SHIPSPD/2;
			 mp_player[0].playframe = SPR_SHIP_RIGHT;
			 break;
		 case DLEFT:
			 mp_player[0].x-=SHIPSPD;
			 mp_player[0].playframe = SPR_SHIP_LEFT;
			 break;
		 case DRIGHT:
			 mp_player[0].x+=SHIPSPD;
			 mp_player[0].playframe = SPR_SHIP_RIGHT;
			 break;
		 case DDOWNRIGHT:
			 mp_player[0].x+=SHIPSPD*2;
			 mp_player[0].y+=SHIPSPD*0.8;
			 mp_player[0].playframe = SPR_SHIP_RIGHT;
			 break;
		 }
		 mp_player[0].scrollTriggers();
	 break;
	 case CMD_SPAWNSPR:
		 //objects[MARK_SPR_NUM].sprite = shipqueue[ShipQueuePtr].flag1;
		 //objects[MARK_SPR_NUM].exists = 1;
		 break;
	 case CMD_REMOVESPR:
		 //objects[MARK_SPR_NUM].sprite = shipqueue[ShipQueuePtr].flag1;
		 //objects[MARK_SPR_NUM].exists = 0;
		 break;
	 case CMD_ENABLESCROLLING:
		 m_scrollingon = true;
		 break;
	 case CMD_DISABLESCROLLING:
		 m_scrollingon = false;
		 break;
	 case CMD_WAIT:
		 break;
	 case CMD_FADEOUT:
		 m_finished = true;
		 break;
	 default: break;
	 }

	 if (m_shipqueue[m_ShipQueuePtr].time > 0) // decrease the time remaining
		 m_shipqueue[m_ShipQueuePtr].time--;
	 else // no time left on this command, go to next cmd
	 {
		 m_ShipQueuePtr++;
	 }
}


CShipFlySys::~CShipFlySys() {
	// TODO Auto-generated destructor stub
}
