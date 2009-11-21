/*
 * CShipFlySys.cpp
 *
 *  Created on: 09.11.2009
 *      Author: gerstrong
 *
 *  This is used for the scene you see in EP1 and EP2
 *  when the player flies with his ship from point A to B.
 *  TODO: This class might be extended to the Tantalus shoot to earth
 *  scene
 */

#include "CShipFlySys.h"

CShipFlySys::CShipFlySys(CPlayer *p_Player, CMap *p_Map)
{
	mp_Map = p_Map;
	mp_player = &p_Player[0]; // in this case the player will be the ship flying home
	m_finished = false;
	m_scrollingon = true;
	m_ShipQueuePtr = 0;
	mp_player[0].playframe = SPR_SHIP_RIGHT;

	// Now, that everything is initialized, create a mark that will be used
	// as ! or ? Sprite
	CObject mark(1);
	mark.m_type = OBJ_YORP;                // doesn't matter
	mark.exists = 0;
	mark.sprite = SPR_QUESTION;

	// keep the question or exclamation mark sprite next to the player
	mark.onscreen = true;

	mp_player[0].mp_object->push_back(mark);

	mp_mark = &(*(p_Player[0].mp_object->end()-1)); // We still need to manipulate it!
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

		 mp_mark->x = mp_player[0].x + (1<<CSF);
		 mp_mark->y = mp_player[0].y - (1<<CSF);
	 break;
	 case CMD_SPAWNSPR:
		 mp_mark->sprite = m_shipqueue[m_ShipQueuePtr].flag1;
		 mp_mark->exists = true;
		 break;
	 case CMD_REMOVESPR:
		 mp_mark->sprite = m_shipqueue[m_ShipQueuePtr].flag1;
		 mp_mark->exists = false;
		 break;
	 case CMD_ENABLESCROLLING:
		 m_scrollingon = true;
		 break;
	 case CMD_DISABLESCROLLING:
		 m_scrollingon = false;
		 break;
	 case CMD_WAIT:
		 break;
	 case CMD_ENDOFQUEUE:
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
	mp_player[0].mp_object->pop_back();
}
